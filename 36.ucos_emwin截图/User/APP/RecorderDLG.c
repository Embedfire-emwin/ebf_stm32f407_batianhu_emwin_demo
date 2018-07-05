/**
  ******************************************************************************
  * @file    RecorderDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   录音的应用窗口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

// USER START (Optionally insert additional includes)
#include "includes.h"
#include "DIALOG.h"
#include "./Bsp/wm8978/bsp_wm8978.h"
#include "string.h"
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
// USER START (Optionally insert additional defines)
/*--------------------  (使用U2C.exe小工具生成)  ------------------------------------*/
#define UTF8_RECORDERDLG  "\xe5\xbd\x95\xe9\x9f\xb3&\xe5\x9b\x9e\xe6\x94\xbe" //录音&回放
#define UTF8_RECORDE      "\xe5\xbd\x95\xe9\x9f\xb3" //录音
#define UTF8_REPLAY       "\xe5\x9b\x9e\xe6\x94\xbe" //回放
#define UTF8_INPUT        "\xe8\xbe\x93\xe5\x85\xa5\xe6\xba\x90:"//输入源
#define UTF8_FREQ         "\xe9\x87\x87\xe6\xa0\xb7\xe7\x8e\x87:"//采样率
#define UTF8_GAIN         "\xe5\xa2\x9e\xe7\x9b\x8a:"//增益
#define UTF8_EAR          "\xe8\x80\xb3\xe6\x9c\xba:"//耳机
#define UTF8_NAME         "\xe5\x90\x8d\xe7\xa7\xb0:"//名称
#define UTF8_VOLUME       "\xe9\x9f\xb3\xe9\x87\x8f:"//音量
#define UTF8_SCHEDULE     "\xe6\x97\xb6\xe9\x97\xb4:"//时间
#define UTF8_STATE        "\xe7\x8a\xb6\xe6\x80\x81"//状态
#define UTF8_IDLE         "\xe7\xa9\xba\xe9\x97\xb2"//空闲
#define UTF8_SDCARDERROR    "SD\xe5\x8d\xa1\xe8\xaf\xbb\xe5\x8f\x96\xe5\xa4\xb1\xe8\xb4\xa5"//SD读取失败
/*-----------------------------------------------------------------------------------*/

#define RECORDERDIR	            "0:/recorder"
#define RECBUF_SIZE             1024*8

// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// USER START (Optionally insert additional static data)
extern GUI_CONST_STORAGE GUI_BITMAP bmback;
extern GUI_CONST_STORAGE GUI_BITMAP bmnext;

extern FATFS   fs[2];													  /* Work area (file system object) for logical drives */
extern FRESULT f_result; 
extern FIL     f_file;
extern UINT    f_num;


/* 播放状态 */
typedef enum
{
	STA_IDLE = 0,	/* 待机状态 */
	STA_RECORDING,/* 录音状态 */
  STA_REPLAYING,/* 放音状态 */
}DEV_REC_STATE;

typedef struct
{
	uint8_t ucInput;			  /* 输入源：0:MIC, 1:线输入 */
  uint8_t ucOutput;       /* 输出设备(录音状态)：0：无输出，1：耳机输出 */
	uint32_t ucFreq;		    /* 采样频率 */
	uint8_t ucGain;			    /* 当前增益 */
	uint8_t ucVolume;			  /* 当前放音音量 */
  uint8_t ucTime;         /* 录音时间 */
  uint32_t uckbps;        /* 波特率   */
  uint32_t uiCursor;		  /* 播放位置 */
	DEV_REC_STATE ucStatus;	/* 录音机状态 */
}REC_TYPE;

static uint8_t rec_count;
static uint8_t rec_index;
static WavHead  rec_wav;
static uint32_t wavsize=0;         /* wav音频数据大小 */
static REC_TYPE Dev_Rec;
static GUI_HMEM hMEM_Rec;
static char *pMEM_Rec;
static uint16_t *buffer1_Rec;
static uint16_t *buffer2_Rec;
static uint8_t Isread_Rec=0;
static uint8_t bufflag_Rec=0;//0 -> use musicbuf[0][]; 1 -> use musicbuf[1][]
static uint8_t timecount;

static const uint16_t recplaybuf[4]={0X0000,0X0000};//2个16位数据,用于录音时I2S Master发送.循环发送0.

// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRecorder[] = {
  { FRAMEWIN_CreateIndirect, "Recorder",   0,              0,   0,   480, 854, 0, 0x0,    0 },
  { RADIO_CreateIndirect,    "Input",      GUI_ID_RADIO0,  40,  65,  100, 70,  0, 0x1e02, 0 },//输入源选择
  { CHECKBOX_CreateIndirect, "EAROutput",  GUI_ID_CHECK0,  40,  180, 100, 20,  0, 0x0,    0 },//耳机输出选择
  { RADIO_CreateIndirect,    "AudioFreq",  GUI_ID_RADIO1,  170, 60,  100, 190, 0, 0x1e06, 0 },//采样率选择    
  { SLIDER_CreateIndirect,   "gain",       GUI_ID_SLIDER2, 340, 90,  30,  150, 8, 0x0,    0 }, //增益调节
  { TEXT_CreateIndirect,     "Gaindata",   GUI_ID_TEXT3,   325, 60,  60,  25,  0, 0x64,   0 }, //增益数字调节 
  { BUTTON_CreateIndirect,   "start",      GUI_ID_BUTTON0, (480-80)/2, 370, 80,  40,  0, 0x0,    0 },//开始录音按钮
  { TEXT_CreateIndirect,     "recname",    GUI_ID_TEXT4,   130, 280, 200, 35,  0, 0x0,    0 },//录制生成文件的名称
  { TEXT_CreateIndirect,     "time",       GUI_ID_TEXT5,   130, 320, 60,  25,  0, 0x64,   0 }, //录制时间长度  
  
  { TEXT_CreateIndirect,     "playname",   GUI_ID_TEXT6,   0, 560, 480, 35,  0, 0x0,    0 }, //播放的录音文件名
  { SLIDER_CreateIndirect,   "Schedule",   GUI_ID_SLIDER1, 50, 610, 380, 30,  0, 0x0,    0 }, //播放进度条
  { TEXT_CreateIndirect,     "playtime",   GUI_ID_TEXT1,   360, 640, 40,  20,  0, 0x64,   0 },  //录音文件当前播放时间
  { TEXT_CreateIndirect,     "/",   				GUI_ID_TEXT8,   380, 640, 40,  20,  0, 0x64,   0 },  //录音文件当前播放时间
  { TEXT_CreateIndirect,     "alltime",    GUI_ID_TEXT2,   400, 640, 40,  20,  0, 0x64,   0 }, //录音文件总时间  
  { SLIDER_CreateIndirect,   "Volume",     GUI_ID_SLIDER0, 100, 690, 300, 30,  0, 0x0,    0 },//播放音量滑动条
  { TEXT_CreateIndirect,     "volumedata", GUI_ID_TEXT0,   405, 695, 30,  20,  0, 0x64,   0 }, //音量百分比
  { BUTTON_CreateIndirect,   "replay",     GUI_ID_BUTTON1, (480-80)/2, 750, 80,  40,  0, 0x0,    0 }, //重播按钮
  { BUTTON_CreateIndirect,   "back",       GUI_ID_BUTTON2, 130, 755, 30,  30,  0, 0x0,    0 }, //上一个
  { BUTTON_CreateIndirect,   "next",       GUI_ID_BUTTON3, 320, 755, 30,  30,  0, 0x0,    0 }, //下一个
  
  { TEXT_CreateIndirect,     "state",      GUI_ID_TEXT7,   0, 480,  480, 30,  0, 0x64,   0 }, //当前状态
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)

static void MusicPlayer_I2S_DMA_TX_Callback(void)
{
	if(Dev_Rec.ucStatus == STA_REPLAYING)
	{
		if(I2Sx_TX_DMA_STREAM->CR&(1<<19)) //当前读取Memory1数据
		{
			bufflag_Rec=0;
		}
		else
		{
			bufflag_Rec=1;
		}
		Isread_Rec=1;
	}
}

static void Recorder_I2S_DMA_RX_Callback(void)
{
	if(Dev_Rec.ucStatus == STA_RECORDING)
	{
//		printf("wavsize->%d\n",wavsize);		
		if(I2Sxext_RX_DMA_STREAM->CR&(1<<19)) 
		{
			bufflag_Rec=0;
		}
		else
		{
			bufflag_Rec=1;
		}
		Isread_Rec=1;
	}
}

static void recorder(const char *recfile,WM_HWIN hWin)
{ 
  char tempstr[30]={0};          
  uint16_t curtime;  
          
  f_result=f_open(&f_file,recfile,FA_CREATE_ALWAYS|FA_WRITE);
  if(f_result!=FR_OK)
	{
		printf("Open wavfile fail!!!->%d\r\n",f_result);
		f_result = f_close (&f_file);
		Dev_Rec.ucStatus=STA_IDLE;
    sprintf(tempstr,"%s%s...",UTF8_IDLE,UTF8_STATE);
    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT7),tempstr);
    BUTTON_SetText(WM_GetDialogItem(hWin, GUI_ID_BUTTON0),"start");
		return;
	}
	printf("当前录音文件 -> %s\n",recfile);
  
  if(Dev_Rec.ucInput==0) //MIC 
	{
		rec_wav.wChannels=1;         /* 缺省MIC单通道 */
	}
	else                    //LINE
	{
    Dev_Rec.ucGain=Dev_Rec.ucGain/9;
		rec_wav.wChannels=2;         /* 缺省线路输入双声道 */
	}
  rec_wav.riff=0x46464952;       /* “RIFF”; RIFF 标志 */
	rec_wav.size_8=0;              /* 文件长度，未确定 */
	rec_wav.wave=0x45564157;       /* “WAVE”; WAVE 标志 */ 
	
	rec_wav.fmt=0x20746d66;        /* “fmt ”; fmt 标志，最后一位为空 */
	rec_wav.fmtSize=16;            /* sizeof(PCMWAVEFORMAT) */ 
	rec_wav.wFormatTag=1;          /* 1 表示为PCM 形式的声音数据 */ 
	/* 每样本的数据位数，表示每个声道中各个样本的数据位数。 */
	rec_wav.wBitsPerSample=16;
	/* 采样频率（每秒样本数） */
	rec_wav.dwSamplesPerSec=Dev_Rec.ucFreq;
	/* 每秒数据量；其值为通道数×每秒数据位数×每样本的数据位数／ 8。 */
	rec_wav.dwAvgBytesPerSec=rec_wav.wChannels*rec_wav.dwSamplesPerSec*rec_wav.wBitsPerSample/8;  
	/* 数据块的调整数（按字节算的），其值为通道数×每样本的数据位值／8。 */
	rec_wav.wBlockAlign=rec_wav.wChannels*rec_wav.wBitsPerSample/8; 
	
	rec_wav.data=0x61746164;       /* “data”; 数据标记符 */
	rec_wav.datasize=0;            /* 语音数据大小 目前未确定*/
  
  f_result=f_write(&f_file,(const void *)&rec_wav,sizeof(rec_wav),&f_num);
  
  /*  初始化并配置I2S  */
	I2S_Stop();
  wm8978_Reset();		/* 复位WM8978到复位状态 */	
  
  	/* 调节放音音量，左右相同音量 */
	wm8978_SetOUT1Volume(Dev_Rec.ucVolume);
  if(Dev_Rec.ucInput == 1)   /* 线输入 */
	{
		/* 配置WM8978芯片，输入为线输入，输出为耳机 */
    if(Dev_Rec.ucOutput==1)
    {
		  wm8978_CfgAudioPath(LINE_ON | ADC_ON,EAR_LEFT_ON | EAR_RIGHT_ON );
    }
    else
    {
      wm8978_CfgAudioPath(LINE_ON | ADC_ON, OUT_PATH_OFF);
    }
	}
	else   /* MIC输入 */
	{
		/* 配置WM8978芯片，输入为Mic，输出为耳机 */
    if(Dev_Rec.ucOutput==1)
    {
		  wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);
    }
    else
    {
      wm8978_CfgAudioPath(MIC_RIGHT_ON | ADC_ON, OUT_PATH_OFF);
    }	
			
	}
  wm8978_SetMicGain(Dev_Rec.ucGain);
  /* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
  
	I2S_GPIO_Config();
	I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,Dev_Rec.ucFreq);
	I2Sxext_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,Dev_Rec.ucFreq);
	
  
  I2Sx_TX_DMA_Init(&recplaybuf[0],&recplaybuf[1],1);
	DMA_ITConfig(I2Sx_TX_DMA_STREAM,DMA_IT_TC,DISABLE);//开启传输完成中断
	
	I2S_DMA_RX_Callback=Recorder_I2S_DMA_RX_Callback;
	I2Sxext_RX_DMA_Init(buffer1_Rec,buffer2_Rec,RECBUF_SIZE);
  	
	I2S_Play_Start();
	I2Sxext_Recorde_Start();
  timecount=0;
  wavsize=0;
  Dev_Rec.uckbps =  Dev_Rec.ucFreq*32/1000;
  while(1)
  {
    while(Isread_Rec==0)
    {
      if(timecount>5)
      {      
        curtime=f_file.fptr/Dev_Rec.uckbps/125;
        sprintf(tempstr,"%02d:%02d",curtime/60,curtime%60);
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5),tempstr);                  //更新时间显示 
        timecount=0;     
      }
      if(tpad_flag)WM_DeleteWindow(hWin);
      if((f_result!=FR_OK)||(Dev_Rec.ucStatus!=STA_RECORDING)||(UserApp_Running==0))
      {        
        I2Sxext_Recorde_Stop();
        I2S_Play_Stop();
        rec_wav.size_8=wavsize+36;
        rec_wav.datasize=wavsize;
        f_result=f_lseek(&f_file,0);
        f_result=f_write(&f_file,(const void *)&rec_wav,sizeof(rec_wav),&f_num);
        f_result=f_close(&f_file);
        sprintf(tempstr,"%s%s...",UTF8_IDLE,UTF8_STATE);
        if(UserApp_Running)TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT7),tempstr);
        I2S_Stop();		/* 停止I2S录音和放音 */
			  wm8978_Reset();	/* 复位WM8978到复位状态 */
        return;
      }
      GUI_Delay(1);
    }
    timecount++;
    Isread_Rec=0;
    if(bufflag_Rec==0)
			f_result=f_write(&f_file,buffer1_Rec,RECBUF_SIZE*2,&f_num);//写入文件							
		else
			f_result=f_write(&f_file,buffer2_Rec,RECBUF_SIZE*2,&f_num);//写入文件
    wavsize+=RECBUF_SIZE*2;
  }
}

static void replay(const char *recfile,WM_HWIN hWin)
{
  uint16_t curtime,alltime;
  char tempstr[30]={0};
  
  f_result=f_open(&f_file,recfile,FA_READ);
	if(f_result!=FR_OK)
	{
		printf("Open wavfile fail!!!->%d\r\n",f_result);
		f_result = f_close (&f_file);
    Dev_Rec.ucStatus=STA_IDLE;
    sprintf(tempstr,"%s%s...",UTF8_IDLE,UTF8_STATE);
    TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT7),tempstr);
		return;
	}
  f_result = f_read(&f_file,&rec_wav,sizeof(WavHead),&f_num);//读取WAV文件头
    /* 初始化全局变量 */
	Dev_Rec.ucFreq =  rec_wav.dwSamplesPerSec;
	Dev_Rec.uckbps =  Dev_Rec.ucFreq*32/1000;
  
  alltime=f_file.fsize/Dev_Rec.uckbps/125;
  sprintf(tempstr,"%02d:%02d",alltime/60,alltime%60);
  TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),tempstr);
  
  SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER1),0);
  
	wm8978_Reset();		/* 复位WM8978到复位状态 */		


	Dev_Rec.uiCursor = 0;		/* 数据指针复位为0，从头开始放音 */

	/* 配置WM8978芯片，输入为DAC，输出为耳机 */
	wm8978_CfgAudioPath(DAC_ON, EAR_LEFT_ON | EAR_RIGHT_ON);

	/* 调节音量，左右相同音量 */
	wm8978_SetOUT1Volume(Dev_Rec.ucVolume);

	/* 配置WM8978音频接口为飞利浦标准I2S接口，16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16);
	
	I2S_GPIO_Config();
	I2Sx_Mode_Config(I2S_Standard_Phillips,I2S_DataFormat_16b,Dev_Rec.ucFreq);
	I2S_DMA_TX_Callback=MusicPlayer_I2S_DMA_TX_Callback;			//回调函数指wav_i2s_dma_callback
	I2S_Play_Stop();
	
	f_result = f_read(&f_file,buffer1_Rec,RECBUF_SIZE*2,&f_num);
	f_result = f_read(&f_file,buffer2_Rec,RECBUF_SIZE*2,&f_num);
	
	I2Sx_TX_DMA_Init(buffer1_Rec,buffer2_Rec,RECBUF_SIZE);
	Isread_Rec=0;
	bufflag_Rec=0;
  timecount=0;
  
  I2S_Play_Start();
  
  while(1)
  {
    while((Isread_Rec==0)&&(Dev_Rec.ucStatus==STA_REPLAYING)&&UserApp_Running)
    {
      if(timecount>=2)      
      { 
        curtime=f_file.fptr/Dev_Rec.uckbps/125;                                        //获取当前播放进度(单位：s)
        sprintf(tempstr,"%02d:%02d",curtime/60,curtime%60);
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),tempstr);                  //更新时间显示 
        SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER1),curtime*255/alltime); //更新时间进度条          
        timecount=0;
      }
      if(tpad_flag)WM_DeleteWindow(hWin);
      GUI_Delay(1);
    }
    timecount++;
    if(Dev_Rec.ucStatus==STA_REPLAYING)
    {
      if(bufflag_Rec==0)
      {
        f_result = f_read(&f_file,buffer1_Rec,RECBUF_SIZE*2,&f_num);	
      }
      else
      {
        f_result = f_read(&f_file,buffer2_Rec,RECBUF_SIZE*2,&f_num);
      }
    }
    Isread_Rec=0;
    if((f_result!=FR_OK)||(f_file.fptr==f_file.fsize)||(Dev_Rec.ucStatus!=STA_REPLAYING)||(UserApp_Running==0))
		{
      printf("播放完或者读取出错退出...\r\n");
      I2S_Play_Stop();
      f_result=FR_OK;
			f_file.fptr=0;
			f_close(&f_file);
			I2S_Stop();		/* 停止I2S录音和放音 */
			wm8978_Reset();	/* 复位WM8978到复位状态 */
      if(UserApp_Running==0)return;
			
      SLIDER_SetValue(WM_GetDialogItem(hWin, GUI_ID_SLIDER1),0);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"00:00");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),"00:00");			
      Dev_Rec.ucStatus=STA_IDLE;
      sprintf(tempstr,"%s%s...",UTF8_IDLE,UTF8_STATE);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT7),tempstr);
			
			return;
		}
  }
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogRecorder(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  char tmp[6];
  char tempstr[20];
  uint8_t value;
  // USER END
  switch (pMsg->MsgId) {
  case WM_DELETE:
		// USER START (Optionally insert additional code for further widget initialization)
		APP_TRACE_DBG(("RecorderDLG delete\n"));	 
    I2S_Stop();		/* 停止I2S录音和放音 */
		wm8978_Reset();	/* 复位WM8978到复位状态 */
//		Flag_ICON2 = 0;
		UserApp_Running = 0;
    tpad_flag=0;
    if(Dev_Rec.ucStatus==STA_RECORDING)
    {
      printf("recording\n");
      I2Sxext_Recorde_Stop();
      I2S_Play_Stop();
      rec_wav.size_8=wavsize+36;
      rec_wav.datasize=wavsize;
      f_result=f_lseek(&f_file,0);
      f_result=f_write(&f_file,(const void *)&rec_wav,sizeof(rec_wav),&f_num);
      f_close(&f_file);
    }
    buffer1_Rec=NULL;
    buffer2_Rec=NULL;
    pMEM_Rec=NULL;
    GUI_ALLOC_Free(hMEM_Rec);
    f_mount(NULL,"0:",1);
		// USER END
	  break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Recorder'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_RECORDERDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'Input'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0);
    RADIO_SetFont(hItem, GUI_FONT_24B_ASCII);
    RADIO_SetText(hItem, "MIC", 0);
    RADIO_SetText(hItem, "LINE", 1);
    RADIO_SetValue(hItem,Dev_Rec.ucInput);
    //
    // Initialization of 'start'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, GUI_FONT_24B_ASCII);
    BUTTON_SetText(hItem,"start");
    //
    // Initialization of 'replay'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
    BUTTON_SetFont(hItem, GUI_FONT_24B_ASCII);
    BUTTON_SetText(hItem,"replay");
    //
    // Initialization of 'Back'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
	  BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmback,5,5);
	  BUTTON_SetText(hItem,"");
    //
    // Initialization of 'Next'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3);
    BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmnext,5,5);
	  BUTTON_SetText(hItem,"");    
    //
    // Initialization of 'EAROutput'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CHECK0);
    CHECKBOX_SetText(hItem, "EAR");
    CHECKBOX_SetFont(hItem, GUI_FONT_24B_ASCII);
    CHECKBOX_SetState(hItem,Dev_Rec.ucOutput);
    //
    // Initialization of 'volumedata'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    sprintf(tmp,"%2d%%",Dev_Rec.ucVolume*100/63);
    TEXT_SetText(hItem,tmp);
    //
    // Initialization of 'playtime'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_TOP);
    TEXT_SetText(hItem, "00:00");
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
	  //
    // Initialization of 'alltime'
    //
	  hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT8);
    TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_TOP);
    TEXT_SetText(hItem, "/");
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    //
    // Initialization of 'alltime'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_TOP);
    TEXT_SetFont(hItem, GUI_FONT_16B_ASCII);
    TEXT_SetText(hItem, "00:00");
    //
    // Initialization of 'AudioFreq'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO1);
    RADIO_SetFont(hItem, GUI_FONT_20B_ASCII);
    RADIO_SetText(hItem, "8KHz", 0);
    RADIO_SetText(hItem, "16KHz", 1);
    RADIO_SetText(hItem, "22KHz", 2);
    RADIO_SetText(hItem, "44KHz", 3);
    RADIO_SetText(hItem, "96KHz", 4);
    RADIO_SetText(hItem, "192KHz", 5);
    //
    // Initialization of 'Gaindata'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    sprintf(tmp,"%d%%",Dev_Rec.ucGain*100/63);
    TEXT_SetText(hItem,tmp);
    //
    // Initialization of 'recname'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT4);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);	
    //
    // Initialization of 'time'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT5);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetText(hItem, "00:00");
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
	  //
    // Initialization of 'playname'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT6);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'state'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT7);
    TEXT_SetFont(hItem, &FONT_XINSONGTI_25);
    TEXT_SetTextAlign(hItem, GUI_TA_CENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem,GUI_ORANGE);
    sprintf(tempstr,"%s%s...",UTF8_IDLE,UTF8_STATE);
    TEXT_SetText(hItem,tempstr);
    //
    // Initialization of 'Volume'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
    SLIDER_SetRange(hItem,1,63);
    SLIDER_SetValue(hItem,35);
    SLIDER_SetNumTicks(hItem,8);
    //
    // Initialization of 'Schedule'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
    SLIDER_SetRange(hItem,0,0xff);
    SLIDER_SetValue(hItem,0);
    SLIDER_SetNumTicks(hItem,0);
	  SLIDER_SetWidth(hItem,5);
    //
    // Initialization of 'gain'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER2);
    SLIDER_SetRange(hItem,1,63);
    SLIDER_SetValue(hItem,Dev_Rec.ucGain);
    SLIDER_SetNumTicks(hItem,0);
	  SLIDER_SetWidth(hItem,5);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_RADIO0: // Notifications sent by 'Input'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        Dev_Rec.ucInput=RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0));
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON0: // Notifications sent by 'start'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(Dev_Rec.ucStatus==STA_IDLE)
        {
          Dev_Rec.ucStatus=STA_RECORDING;
          sprintf(tempstr,"%s%s...",UTF8_RECORDE,UTF8_STATE);
          BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0),"stop");
        }
        else if(Dev_Rec.ucStatus==STA_RECORDING)
        {
          Dev_Rec.ucStatus=STA_IDLE;
          sprintf(tempstr,"%s%s...",UTF8_IDLE,UTF8_STATE);
          BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0),"start");
        }
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT7),tempstr);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'replay'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(Dev_Rec.ucStatus==STA_IDLE)
        {
          Dev_Rec.ucStatus=STA_REPLAYING;
          sprintf(tempstr,"%s%s...",UTF8_REPLAY,UTF8_STATE);
        }
        else if(Dev_Rec.ucStatus==STA_REPLAYING)
        {
          Dev_Rec.ucStatus=STA_IDLE;
          sprintf(tempstr,"%s%s...",UTF8_IDLE,UTF8_STATE);
        }        
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT7),tempstr);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON2: // Notifications sent by 'Back'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(rec_index==0)
          rec_index=rec_count-1;
        else
          rec_index--;        
        sprintf(tempstr,"rec%03d.wav",rec_index);
	      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT6),tempstr);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON3: // Notifications sent by 'Next'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        rec_index++;
        if(rec_index>rec_count-1)
          rec_index=1;
        sprintf(tempstr,"rec%03d.wav",rec_index);
	      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT6),tempstr);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;      
    case GUI_ID_CHECK0: // Notifications sent by 'EAROutput'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        Dev_Rec.ucOutput=CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, GUI_ID_CHECK0));
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_SLIDER0: // Notifications sent by 'Volume'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        Dev_Rec.ucVolume=SLIDER_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0));
        sprintf(tmp,"%2d%%",Dev_Rec.ucVolume*100/63);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0),tmp);
        if(Dev_Rec.ucStatus==STA_REPLAYING)
        {
          wm8978_OutMute(0);
          /* 调节音量，左右相同音量 */
          wm8978_SetOUT1Volume(Dev_Rec.ucVolume);
        }
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_SLIDER1: // Notifications sent by 'Schedule'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_RADIO1: // Notifications sent by 'AudioFreq'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        value=RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO1));
        switch(value)
        {
          case 0:
            Dev_Rec.ucFreq=I2S_AudioFreq_8k;
            break;
          case 1:
            Dev_Rec.ucFreq=I2S_AudioFreq_16k;
            break;
          case 2:
            Dev_Rec.ucFreq=I2S_AudioFreq_22k;
            break;
          case 3:
            Dev_Rec.ucFreq=I2S_AudioFreq_44k;
            break;
          case 4:
            Dev_Rec.ucFreq=I2S_AudioFreq_96k;
            break;
          case 5:
            Dev_Rec.ucFreq=I2S_AudioFreq_192k;
            break;
        }
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_SLIDER2: // Notifications sent by 'gain'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        value=SLIDER_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER2));
        sprintf(tmp,"%d%%",value*100/63);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3),tmp);
        if(Dev_Rec.ucInput==0)
        {
          Dev_Rec.ucGain=value;
        }
        else
        {
          Dev_Rec.ucGain=value/9;
        }
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  case WM_PAINT:
	  GUI_SetColor(GUI_BLUE);  
    GUI_DrawRect(10,10,480-20,430);
    GUI_DrawRect(10,450,480-20,530);
    GUI_DrawRect(10,550,480-20,800);
    GUI_SetColor(GUI_LIGHTBLUE);
	  GUI_SetFont(&FONT_XINSONGTI_25);
    GUI_DispStringAt(UTF8_RECORDE,20,0);
    GUI_DispStringAt(UTF8_STATE,20,440);  
    GUI_DispStringAt(UTF8_REPLAY,20,540);  
  
    GUI_SetColor(GUI_LIGHTGREEN);  
    GUI_SetFont(&FONT_XINSONGTI_19);
    GUI_DispStringAt(UTF8_INPUT,40,35);
    GUI_DispStringAt(UTF8_FREQ,170,35);
    GUI_DispStringAt(UTF8_GAIN,340,30);  
    GUI_DispStringAt(UTF8_EAR,40,150);
    GUI_DispStringAt(UTF8_NAME,50,287);
    GUI_DispStringAt(UTF8_SCHEDULE,50,322);  
    GUI_DispStringAt(UTF8_SCHEDULE,50,590);
    GUI_DispStringAt(UTF8_VOLUME,50,670);
    break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateRecorder
*/
void FUN_ICON2Clicked(void) 
{
  WM_HWIN hWin;
  uint8_t i;
  char str[30]={0};
  APP_TRACE_DBG(("RecorderDLG create\n"));
  Dev_Rec.ucVolume=35;
  Dev_Rec.ucOutput=1;
  Dev_Rec.ucInput=0;
  Dev_Rec.ucFreq=I2S_AudioFreq_8k;
  Dev_Rec.ucGain=40;
  Dev_Rec.ucTime=0;
  Dev_Rec.ucStatus=STA_IDLE;
  
	/* 挂载文件系统 -- SD卡 */
  f_result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive */
	if(f_result!=FR_OK) 
    bsp_result |= BSP_SD;
  else 
  {
    bsp_result &=~BSP_SD;
    com_createdir(RECORDERDIR);
  }
  
  hWin = GUI_CreateDialogBox(_aDialogCreateRecorder, GUI_COUNTOF(_aDialogCreateRecorder), _cbDialogRecorder, WM_HBKWIN, 0, 0);
  
  /* 申请一块内存空间 并且将其清零 */
  hMEM_Rec = GUI_ALLOC_AllocZero(RECBUF_SIZE*4);

  /* 将申请到内存的句柄转换成指针类型 */
  pMEM_Rec = GUI_ALLOC_h2p(hMEM_Rec);
  if(pMEM_Rec==NULL)
  {
    bsp_result|=BSP_ALLOC;
    buffer1_Rec=NULL;
    buffer2_Rec=NULL;
  }
  else
  {
    buffer1_Rec=(uint16_t *)pMEM_Rec;
    buffer2_Rec=(uint16_t *)(pMEM_Rec+RECBUF_SIZE*2);
  }
  
  /* 检测WM8978芯片，此函数会自动配置CPU的GPIO */
  if (wm8978_Init()==0)
     bsp_result|=BSP_WM8978;
  else
    bsp_result&=~BSP_WM8978;
  
  if((bsp_result&BSP_SD)||(bsp_result&BSP_WM8978))
  {		
    GUI_Delay(100);
    if(bsp_result&BSP_WM8978)
      sprintf(str,"The WM8978 drive cannot work!");  
    else if(bsp_result&BSP_SD)
      sprintf(str,UTF8_SDCARDERROR); 
    else
      sprintf(str,"one drive cannot work!");
    ErrorDialog(hWin,"Recorder Error",str);
    while(1)
    {
       if(tpad_flag)WM_DeleteWindow(hWin);
       if(!UserApp_Running)return;
       GUI_Delay(10);
    }
  }
  i=1;
  while(i<0xff)
  {
    sprintf(str,"%s/rec%03d.wav",RECORDERDIR,i);
    f_result=f_open(&f_file,(const TCHAR*)str,FA_READ);
    //printf("f_open_res:%d\n",result);
    if(f_result==FR_NO_FILE)break;	
    else
        f_close(&f_file);
    i++;
  }
  rec_count=i;
  rec_index=1;
  sprintf(str,"rec%03d.wav",rec_count);
	TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),str);
  sprintf(str,"rec%03d.wav",rec_index);
	TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6),str);
  
  while(UserApp_Running)
  {
    if(Dev_Rec.ucStatus==STA_RECORDING)
    {
      rec_index=rec_count;
      sprintf(str,"rec%03d.wav",rec_count);
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT4),str);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT5),"00:00");
      GUI_Delay(10);    
      sprintf(str,"%s/rec%03d.wav",RECORDERDIR,rec_count);
			recorder(str,hWin);	
      rec_count++;
    }
    else if(Dev_Rec.ucStatus==STA_REPLAYING)
    {      
      sprintf(str,"rec%03d.wav",rec_index);
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT6),str);		
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"00:00");
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),"00:00");       
			sprintf(str,"%s/rec%03d.wav",RECORDERDIR,rec_index);
			replay(str,hWin);
    }
    if(tpad_flag)WM_DeleteWindow(hWin);
    GUI_Delay(10);
  }
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
