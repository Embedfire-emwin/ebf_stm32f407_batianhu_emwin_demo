/**
  ******************************************************************************
  * @file    USBDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   模拟U盘的应用窗口
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

#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usb_bsp.h"
// USER END
/**************************************************************************************
*
*       Defines
*
***************************************************************************************
*/
// USER START (Optionally insert additional defines)
/*--------------------  (使用U2C.exe小工具生成)  ------------------------------------*/
#define UTF8_USBDLG        "\xe6\xa8\xa1\xe6\x8b\x9fU\xe7\x9b\x98"//模拟U盘
#define UTF8_USEBEFORE     "\xe4\xbd\xbf\xe7\x94\xa8\xe5\x89\x8d\xe7\xa1\xae\xe4\xbf\x9dUSB\xe7\xba\xbf\xe5\xb7\xb2\xe6\x8e\xa5\xe5\x85\xa5" //使用前确保USB线已接入

/*-----------------------------------------------------------------------------------*/

// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
extern GUI_CONST_STORAGE GUI_BITMAP bmusbicon;

__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev __ALIGN_END ;

static uint8_t usbd_msd_flag;

// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateUSB[] = {
  { FRAMEWIN_CreateIndirect, "USB",0, 0, 0, 480, 854, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "OPEN", GUI_ID_BUTTON0, (480-160)/2, (854-35-160)/2, 160, 160, 0, 0x0, 0 },
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
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogUSB(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
	  case WM_DELETE:
		APP_TRACE_DBG(("USBDLG delete\n"));
//		Flag_ICON11 = 0;
		UserApp_Running = 0;
		tpad_flag=0;
    LED1_OFF;
    DCD_DevDisconnect(&USB_OTG_dev);
    USB_OTG_STOP();
	break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'USB'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_USBDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'OPEN'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetBitmapEx(hItem,BUTTON_BI_UNPRESSED,&bmusbicon,5,5);
    BUTTON_SetText(hItem, "");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_BUTTON0: // Notifications sent by 'OPEN'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)     
	      usbd_msd_flag=1;	   
        WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0));
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
#if   LCD_NUM_LAYERS == 1 
		GUI_SetBkColor(APPBKCOLOR);
#elif LCD_NUM_LAYERS == 2
     GUI_SetBkColor(GUI_TRANSPARENT);
#endif
		 GUI_Clear();
     GUI_SetColor(GUI_LIGHTGREEN);  
     GUI_SetFont(&FONT_XINSONGTI_25);
     GUI_DispStringHCenterAt(UTF8_USEBEFORE,480/2,300);
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
*       CreateUSB
*/
void FUN_ICON11Clicked(void)
{
  uint16_t i=0;
	WM_HWIN hWin;
  usbd_msd_flag=0;
  APP_TRACE_DBG(("USBDLG create\n"));
	hWin=GUI_CreateDialogBox(_aDialogCreateUSB, GUI_COUNTOF(_aDialogCreateUSB), _cbDialogUSB, WM_HBKWIN, 0, 0);
	
  
	while(UserApp_Running)
	{
    if(usbd_msd_flag==1)
    {
      /*!< At this stage the microcontroller clock setting is already configured, 
      this is done through SystemInit() function which is called from startup
      file (startup_stm32fxxx_xx.s) before to branch to application main.
      To reconfigure the default setting of SystemInit() function, refer to
      system_stm32fxxx.c file
      */       
      USBD_Init(&USB_OTG_dev,
                USB_OTG_FS_CORE_ID,
                &USR_desc,
                &USBD_MSC_cb, 
                &USR_cb);
      usbd_msd_flag=0;
    }
    if(i>200)
    {
      LED1_TOGGLE;
      i=0;       
    }
    i++;
    if(tpad_flag)WM_DeleteWindow(hWin);  
		GUI_Delay(5);
	}
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
