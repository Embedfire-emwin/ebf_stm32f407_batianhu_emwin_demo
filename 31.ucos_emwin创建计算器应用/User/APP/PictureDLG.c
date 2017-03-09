/**
  ******************************************************************************
  * @file    PictureDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   图片浏览器的应用窗口
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

// USER START (Optionally insert additional includes)
#include "includes.h"
#include "DIALOG.h"
#include "string.h"
// USER END

/**************************************************************************************
*
*       Defines
*
***************************************************************************************
*/

// USER START (Optionally insert additional defines)
/*--------------------  (使用U2C.exe小工具生成)  ------------------------------------*/
#define UTF8_PICTUREDLG        "\xe7\x85\xa7\xe7\x89\x87"//照片

/*-----------------------------------------------------------------------------------*/

#define g_ucPicNum    8
#define PATHLENTH	    40
#define _DF1S	        0x81
#define imageWinX_S		10
#define imageWinY_S    (35+75)
#define imageWinX_L   (480-2*imageWinX_S)
#define imageWinY_L   (854-35-2*(imageWinY_S-35))
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
extern FATFS   fs[2];													  /* Work area (file system object) for logical drives */

FRESULT f_result; 
FIL     f_file;
UINT    f_num;

//图片空间，最大显示多大的图片
#define PICTURE_BUFFER_SIZE 			(500*1024)

// USER START (Optionally insert additional static data)
static char BMP_Name[g_ucPicNum][PATHLENTH];
static int8_t BMP_Num=0;
static int8_t BMP_Index=0;
static char JPG_Name[g_ucPicNum][PATHLENTH];
static int8_t JPG_Num=0;
static int8_t JPG_Index=0;
static char GIF_Name[g_ucPicNum][PATHLENTH];
static int8_t GIF_Num=0;
static int8_t GIF_Index=0;
static uint8_t g_ucPicType = 0;

static char picpath[PATHLENTH*2]="1:/Picture";

static GUI_HMEM hPicMemory;
char *_acBuffer;
GUI_GIF_INFO InfoGif1;
GUI_GIF_IMAGE_INFO InfoGif2;
GUI_JPEG_INFO JpegInfo;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreatePicture[] = {
  { FRAMEWIN_CreateIndirect, "PICTURE",               0, 0,   0,   480, 854, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "BACK",     GUI_ID_BUTTON0, 480/2-25,   20, 50,  40,  0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "NEXT",     GUI_ID_BUTTON1, 480/2-25, 750, 50,  40,  0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "TYPE",     GUI_ID_BUTTON2, 40,   20, 50,  40,  0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "ZOOM IN",  GUI_ID_BUTTON3, 40, 750, 40,  40,  0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "ZOOM OUT", GUI_ID_BUTTON4, 100, 750, 40,  40,  0, 0x0, 0 },
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
/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path) 
{ 
  FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
  FILINFO fno; 
  DIR dir; 
  int i; 
  char *fn;
#if _USE_LFN 
  static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
  fno.lfname = lfn; 
  fno.lfsize = sizeof(lfn); 
#endif 
	
  res = f_opendir(&dir, path); //打开目录
  if (res == FR_OK) 
	{ 
    i = strlen(path); 
    while(1)
		{ 
      res = f_readdir(&dir, &fno); 										//读取目录下的内容
      if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
      fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
      fn = fno.fname; 
#endif 
      if (*fn == '.') continue; 											//点表示当前目录，跳过			
      if (fno.fattrib & AM_DIR) 
			{ 																							//目录，递归读取
         sprintf(&path[i], "/%s", fn); 							//合成完整目录名
         res = scan_files(path);											//递归遍历 
         if (res != FR_OK)	break; 																		//打开失败，跳出循环
         path[i] = 0; 
      } 
			else 
			{ 
				if(strstr(fn,".BMP")||strstr(fn,".bmp"))
				{
					if (strlen(path)+strlen(fn)<PATHLENTH)
					{			
            sprintf(BMP_Name[BMP_Num], "%s/%s", path, fn);
						BMP_Num++;//记录文件个数
						if(BMP_Num>=g_ucPicNum)BMP_Num=g_ucPicNum-1;
					}
				}
				if(strstr(fn,".JPG")||strstr(fn,".jpg"))
				{
					if (strlen(path)+strlen(fn)<PATHLENTH)
					{
            sprintf(JPG_Name[JPG_Num], "%s/%s", path, fn);
						JPG_Num++;//记录文件个数
						if(JPG_Num>=g_ucPicNum)JPG_Num=g_ucPicNum-1;
					}
				}
				if(strstr(fn,".GIF")||strstr(fn,".gif"))
				{
					if (strlen(path)+strlen(fn)<PATHLENTH)
					{	
            sprintf(GIF_Name[GIF_Num], "%s/%s", path, fn);            
						GIF_Num++;//记录文件个数
						if(GIF_Num>=g_ucPicNum)GIF_Num=g_ucPicNum-1;
					}
		    }
      }//else
    } //for
  } 
  return res; 
}
// USER START (Optionally insert additional static code)
static uint8_t  _loadbuffer(const char * sFilename) 
{
	OS_ERR      	err;
	
	OSSchedLock(&err);	
	f_result = f_open(&f_file, sFilename, FA_READ);
	if ((f_result != FR_OK)||(f_file.fsize>PICTURE_BUFFER_SIZE))
	{
    _acBuffer[0]='\0';        
		OSSchedUnlock(&err);
		return 0;
	}
	f_result = f_read(&f_file, _acBuffer, f_file.fsize, &f_num);
	if(f_result != FR_OK)
  {
		OSSchedUnlock(&err);
		return 0;
  }
  f_close(&f_file);	
	OSSchedUnlock(&err);
	return 1;
	
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogPicture(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
//  OS_ERR      	err;
  // USER START (Optionally insert additional variables)
	static int bmp_amred = 100;
  static int jpg_amred = 100;
  // USER END

  switch (pMsg->MsgId) {
	case WM_DELETE:
		APP_TRACE_DBG(("PictureDLG delete\n"));
//		Flag_ICON10 = 0;
		BMP_Index=0;
		JPG_Index=0;
		GIF_Index=0;
		UserApp_Running = 0;
    _acBuffer=NULL;
    GUI_ALLOC_Free(hPicMemory);  
		tpad_flag=0;
	break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'PICTURE'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_PICTUREDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'BACK'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
		BUTTON_SetText(hItem,"<<<");
    //
    // Initialization of 'NEXT'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
		BUTTON_SetText(hItem,">>>");
		//
    // Initialization of 'TYPE'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
		BUTTON_SetText(hItem,"BMP");
		//
    // Initialization of 'ZOOM IN'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
		BUTTON_SetText(hItem,"++");
		//
    // Initialization of 'ZOOM OUT'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON4);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
		BUTTON_SetText(hItem,"--");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_BUTTON0: // Notifications sent by 'BACK'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        GUI_SetBkColor(APPBKCOLOR);
        GUI_Clear();
        if(g_ucPicType == 0)
        {
          BMP_Index--;
          if(BMP_Index < 0)BMP_Index=BMP_Num-1;
          if(_loadbuffer(BMP_Name[BMP_Index])==0)break;
//          OSSchedLock(&err);			
          GUI_BMP_Draw(_acBuffer,
                 (imageWinX_L - GUI_BMP_GetXSize(_acBuffer))/2, 
                 (imageWinY_L - GUI_BMP_GetYSize(_acBuffer))/2);
//          OSSchedUnlock(&err);																	
        }
        else if(g_ucPicType == 1)
        {
          JPG_Index--;
          if(JPG_Index < 0)JPG_Index=JPG_Num-1;
          _loadbuffer(JPG_Name[JPG_Index]);
//          OSSchedLock(&err);
          GUI_JPEG_GetInfo(_acBuffer, f_file.fsize, &JpegInfo);
          
          GUI_JPEG_DrawScaled(_acBuffer,
                              f_file.fsize, 
                              (imageWinX_L - JpegInfo.XSize)/2, 
                              (imageWinY_L - JpegInfo.YSize)/2,
                              100,
                              100);
//          OSSchedUnlock(&err);						
        }
        else
        {
          GIF_Index--;
          if(GIF_Index < 0)GIF_Index=GIF_Num-1;
          _loadbuffer(GIF_Name[GIF_Index]);
          GUI_GIF_GetInfo(_acBuffer, f_file.fsize, &InfoGif1);
        }
        //IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, GUI_ID_IMAGE0),);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'NEXT'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        GUI_SetBkColor(APPBKCOLOR);
        GUI_Clear();
        if(g_ucPicType == 0)
        {
          BMP_Index++;
          if(BMP_Index >= BMP_Num)BMP_Index=0;
         
					for(;BMP_Index<BMP_Num;BMP_Index++)
					{
						if(_loadbuffer(BMP_Name[BMP_Index])==1)
						{
				//      OSSchedLock(&err);
							GUI_BMP_Draw(_acBuffer,
										(imageWinX_L - GUI_BMP_GetXSize(_acBuffer))/2, 
										(imageWinY_L - GUI_BMP_GetYSize(_acBuffer))/2);
							break;
						}
					}
//          OSSchedUnlock(&err);																	
        }
        else if(g_ucPicType == 1)
        {
          JPG_Index++;
          if(JPG_Index >= JPG_Num)JPG_Index=0;
          _loadbuffer(JPG_Name[JPG_Index]);

//          OSSchedLock(&err);
          GUI_JPEG_GetInfo(_acBuffer, f_file.fsize, &JpegInfo);
          GUI_JPEG_DrawScaled(_acBuffer,
                              f_file.fsize, 
                              (imageWinX_L - JpegInfo.XSize)/2, 
                              (imageWinY_L - JpegInfo.YSize)/2,
                              100,
                              100);
//          OSSchedUnlock(&err);						
        }
        else
        {
          GIF_Index++;
          if(GIF_Index >= GIF_Num)GIF_Index=0;
          _loadbuffer(GIF_Name[GIF_Index]);
          GUI_GIF_GetInfo(_acBuffer, f_file.fsize, &InfoGif1);
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
	  case GUI_ID_BUTTON2: // Notifications sent by 'TYPE'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(g_ucPicType == 0)
        {
          g_ucPicType = 1;
          BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2), "JPG");							
        }
        else if(g_ucPicType == 1)
        {
          g_ucPicType = 2;
          GUI_SetBkColor(APPBKCOLOR);
          GUI_Clear();
          _loadbuffer(GIF_Name[GIF_Index]);
          GUI_GIF_GetInfo(_acBuffer, f_file.fsize, &InfoGif1);
          BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2), "GIF");
        }
        else
        {
          g_ucPicType = 0;
          BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2), "BMP");    
        }
        //IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, GUI_ID_IMAGE0),);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
	  case GUI_ID_BUTTON3: // Notifications sent by 'ZOOM IN'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(g_ucPicType == 2)break;
        if(g_ucPicType == 0)
        {
          bmp_amred += 10;
//          OSSchedLock(&err);
          GUI_BMP_DrawScaled(_acBuffer,
                             (imageWinX_L - GUI_BMP_GetXSize(_acBuffer)*bmp_amred/100)/2, 
                             (imageWinY_L - GUI_BMP_GetYSize(_acBuffer)*bmp_amred/100)/2,
                             bmp_amred, 
                             100);
//          OSSchedUnlock(&err);																	
        }
        else if(g_ucPicType == 1)
        {
          jpg_amred +=10;
//          OSSchedLock(&err);
          GUI_JPEG_DrawScaled(_acBuffer,
                              f_file.fsize, 
                              (imageWinX_L - JpegInfo.XSize*jpg_amred/100)/2, 
                              (imageWinY_L - JpegInfo.YSize*jpg_amred/100)/2,
                              jpg_amred,
                                    100); 
//          OSSchedUnlock(&err);						
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
	  case GUI_ID_BUTTON4: // Notifications sent by 'ZOOM OUT'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(g_ucPicType == 2)break;
        GUI_SetBkColor(APPBKCOLOR);
        GUI_Clear();
        if(g_ucPicType == 0)
        {
          bmp_amred -= 10;
                if(bmp_amred < 0) bmp_amred = 10; 
//          OSSchedLock(&err);
          GUI_BMP_DrawScaled(_acBuffer,
                             (imageWinX_L - GUI_BMP_GetXSize(_acBuffer)*bmp_amred/100)/2, 
                             (imageWinY_L - GUI_BMP_GetYSize(_acBuffer)*bmp_amred/100)/2,
                             bmp_amred, 
                             100);
//          OSSchedUnlock(&err);																	
        }
        else if(g_ucPicType == 1)
        {
          jpg_amred -= 10;
                if(jpg_amred < 0) jpg_amred = 10;
//          OSSchedLock(&err);
          GUI_JPEG_DrawScaled(_acBuffer,
                              f_file.fsize, 
                              (imageWinX_L - JpegInfo.XSize*jpg_amred/100)/2, 
                              (imageWinY_L - JpegInfo.YSize*jpg_amred/100)/2,
                              jpg_amred,
                              100); 
//          OSSchedUnlock(&err);						
        }
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
*       CreatePicture
*/
void FUN_ICON10Clicked(void)
{
	WM_HWIN hWin;
	WM_HWIN himage;
	int32_t freeByte;
	uint8_t i=0;
//	OS_ERR  err;
	g_ucPicType = 0;
	APP_TRACE_DBG(("PictureDLG create\n"));
	com_createdir(picpath);
	hWin=GUI_CreateDialogBox(_aDialogCreatePicture, GUI_COUNTOF(_aDialogCreatePicture), _cbDialogPicture, WM_HBKWIN, 0, 0);
	GUI_Delay(10);
	himage=WM_CreateWindowAsChild(imageWinX_S,imageWinY_S,imageWinX_L,imageWinY_L,hWin,WM_CF_SHOW,NULL,0);
	
	WM_SelectWindow(himage);
  GUI_SetBkColor(APPBKCOLOR);
  GUI_Clear();
	
	freeByte = GUI_ALLOC_GetNumFreeBytes();
	APP_TRACE_DBG(("freeByte=%d\n",freeByte));

  hPicMemory = GUI_ALLOC_AllocZero(PICTURE_BUFFER_SIZE);
  _acBuffer = GUI_ALLOC_h2p(hPicMemory);
  if(_acBuffer==NULL)  bsp_result|=BSP_ALLOC;
  BMP_Num=0;
	JPG_Num=0;
	GIF_Num=0;
	scan_files(picpath);
	
	freeByte = GUI_ALLOC_GetNumFreeBytes();
	APP_TRACE_DBG(("freeByte=%d\n",freeByte));
	APP_TRACE_DBG(("BMP_NUM=%d,JPG_NUM=%d,GIF_NUM=%d\n",BMP_Num,JPG_Num,GIF_Num));

	if(BMP_Num)
	{
		//显示一幅小于PICTURE_BUFFER_SIZE的图像
		for(BMP_Index = 0;BMP_Index<BMP_Num;BMP_Index++)
		{
			if(_loadbuffer(BMP_Name[BMP_Index])==1)
			{
	//      OSSchedLock(&err);
				GUI_BMP_Draw(_acBuffer,
							(imageWinX_L - GUI_BMP_GetXSize(_acBuffer))/2, 
							(imageWinY_L - GUI_BMP_GetYSize(_acBuffer))/2);
				break;
	//      OSSchedUnlock(&err);
			}
		}
	}
	while(UserApp_Running)
	{
		if(g_ucPicType == 2)
		{
			if(i < InfoGif1.NumImages)
			{                                    
//				OSSchedLock(&err);
				//GUI_GIF_GetImageInfo(_acBuffer2, file.fsize, &InfoGif2, i);
				GUI_GIF_DrawSub(_acBuffer, 
												f_file.fsize, 
												(imageWinX_L - InfoGif1.xSize)/2, 
												(imageWinY_L - InfoGif1.ySize)/2, 
												i++);                             
//				OSSchedUnlock(&err);
				if(InfoGif2.Delay == 0)				
					GUI_Delay(100);
				else
					GUI_Delay(InfoGif2.Delay*10);                      						 
			}
			else
			{
				i = 0;
			}
			if(tpad_flag)WM_DeleteWindow(hWin);
		}
		else
		{
			if(tpad_flag)WM_DeleteWindow(hWin);
			GUI_Delay(10); 
		}    
	}
}

// USER START (Optionally insert additional public code)
// USER END
/*************************** End of file ****************************/
