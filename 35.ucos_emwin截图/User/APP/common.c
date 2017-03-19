#include "common.h"
#include "ff.h"
#include "GUI.h"
#include "string.h"
#include "DIALOG.h"
#include "includes.h"

/*********************************************************************
*
*       data
*
**********************************************************************
*/
//uint8_t comdata[COMDATA_SIZE];


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
void com_data2null(uint8_t *data,uint32_t length)
{
	uint32_t i=0;
	while(i<length)
	{
		data[i]='\0';
		++i;
	}
}

void com_gbk2utf8(const char *src, char *str)
{
	uint32_t j=0,k=0;
	uint16_t gbkdata=0;
	uint16_t UCbuffer[1024]={0};
	for(j=0,k=0;src[j]!='\0';k++)
	{
		if((uint8_t)src[j]>0x80)
		{
			gbkdata=src[j+1]+src[j]*256;
			UCbuffer[k]=ff_convert(gbkdata,1);
			j+=2;
		}
		else
		{
			UCbuffer[k]=0x00ff&src[j];
			j+=1;
		}
	}
	UCbuffer[k]='\0';
	GUI_UC_ConvertUC2UTF8(UCbuffer,2*k+2,str,k*3);
}



void com_utf82gbk(const char *src, char *str)
{
	uint32_t k=0,j=0,i=0;
	uint16_t buffer[1024]={0};
	uint16_t gbkdata=0;
	k=strlen(src);
	com_data2null((uint8_t *)buffer,k);
	GUI_UC_ConvertUTF82UC(src,k,buffer,k);
	j=0;
	i=0;
	while(buffer[i]!='\0'&&j<k)
	{
		if(buffer[i]>0x80)
		{
			gbkdata=ff_convert(buffer[i],0);
			str[j]=(char)(gbkdata>>8);
			str[j+1]=(char)gbkdata;
			j+=2;
		}
		else
		{
			str[j]=(char)buffer[i];
			j+=1;
		}
		i++;
	}
	str[j]='\0';
	str[j+1]='\0';
}

void com_createdir(const char *dir)
{
	FRESULT res;
	DIR recdir;
	res=f_opendir(&recdir,dir);
	while(res!=FR_OK)
	{
		f_mkdir(dir);
		res=f_opendir(&recdir,dir);
	}
}


#define ERROR_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ERROR_BUTTON_0   (GUI_ID_USER + 0x01)
#define ERROR_BUTTON_1   (GUI_ID_USER + 0x02)
#define UTF8_CLOSE       "\xe9\x80\x80\xe5\x87\xba"//关闭
#define UTF8_REBOOT      "\xe9\x87\x8d\xe5\x90\xaf"//重启
static char *ErrorStr[2];
static const GUI_WIDGET_CREATE_INFO _aDialogCreateError[] = {
  { FRAMEWIN_CreateIndirect, "Error",  ERROR_FRAMEWIN_0,  0,   0,   400, 160, 0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "CLOSE",  ERROR_BUTTON_0,    110,  80,  60,  30,  0, 0x0, 0 },
  { BUTTON_CreateIndirect,   "Reboot", ERROR_BUTTON_1,    230, 80,  60,  30,  0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};
static void _cbDialogError(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'CLOSE'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ERROR_BUTTON_0);
    BUTTON_SetFont(hItem, &FONT_XINSONGTI_25);
    BUTTON_SetText(hItem, UTF8_CLOSE);
    //
    // Initialization of 'Reboot'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ERROR_BUTTON_1);
    BUTTON_SetFont(hItem, &FONT_XINSONGTI_25);
    BUTTON_SetText(hItem, UTF8_REBOOT);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    if(NCode==WM_NOTIFICATION_RELEASED)
    {
      switch(Id)
      {
        case ERROR_BUTTON_0: // Notifications sent by 'CLOSE'
          WM_DeleteWindow(WM_GetParent(pMsg->hWin));
          break;
        case ERROR_BUTTON_1: // Notifications sent by 'Reboot'
          Bsp_Soft_Reset();//WM_DeleteWindow(WM_GetParent(pMsg->hWin));
          break;
      }
    }    
    break;
  // USER START (Optionally insert additional message handling)
  case WM_PAINT:
      GUI_SetColor(GUI_RED);
      GUI_SetFont(&FONT_XINSONGTI_25);
      GUI_DispStringHCenterAt(ErrorStr[0],200,5);      
      GUI_DispStringHCenterAt(ErrorStr[1],200,35);
   break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

void ErrorDialog(WM_HWIN hParent,char *string1,char *string2)
{
  WM_HWIN hWin;
  ErrorStr[0]=string1;
  ErrorStr[1]=string2;
  hWin=GUI_CreateDialogBox(_aDialogCreateError,GUI_COUNTOF(_aDialogCreateError),_cbDialogError,hParent,40,300);
  WM_MakeModal(hWin);
  WM_SetStayOnTop(hWin, 1);
}



extern FATFS   fs[2];													  /* Work area (file system object) for logical drives */

extern FRESULT f_result; 
extern FIL     f_file;
extern UINT    f_num;


/**
  *****************************************************************************
  * @Name   : emWin向文件写入一个字节数据
  *
  * @Brief  : none
  *
  * @Input  : Data: 要写入的数据
  *           *p:   路径
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
static void _WriteByte2File(U8 Data, void * p)
{
        UINT nWrite = 0;
         
       f_result = f_write((FIL *)p, (TCHAR *)&Data, 1, &nWrite);
}
 
/**
  *****************************************************************************
  * @Name   : emWin得到一幅图片并写入存储器
  *
  * @Brief  : none
  *
  * @Input  : *FileName: 文件名称
  *           (x0, y0):  起点坐标
  *           xSize:     图片X方向大小
  *           ySize:     图片Y方向大小
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void emWin_CreateBMPPicture(uint8_t * FileName, int x0, int y0, int xSize, int ySize)
{
   static uint8_t init_flag = 0;
	
	//初始化SD卡
	if(init_flag == 0)
	{
		f_mount(&fs[1],"0:",1);
		init_flag =1;
	}
	//
	//创建一个文件
	//
	f_result = f_open(&f_file, (const TCHAR *)FileName, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);

	/* 新建文件之后要先关闭再打开才能写入 */
	f_result =f_close(&f_file);

	f_result = f_open( &f_file , (char*)FileName,  FA_OPEN_EXISTING | FA_WRITE);

	GUI_BMP_SerializeEx(_WriteByte2File, x0, y0, xSize, ySize, &f_file);
	f_result = f_close(&f_file);  //写完，关闭文件

}


/*---------------------- end of file --------------------------*/
