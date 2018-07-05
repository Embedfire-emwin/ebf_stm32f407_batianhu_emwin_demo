/**
  ******************************************************************************
  * @file    rgbledDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   RGB灯的应用窗口
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
#include "includes.h"
#include "./led/bsp_color_led.h"
#include "./adc/bsp_adc.h"

//#include  "app.h"
// USER START (Optionally insert additional includes)
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
// USER START (Optionally insert additional defines)
/*--------------------  (使用U2C.exe小工具生成)  ------------------------------------*/
/*
*  U2C.exe小工具使用方法：新建一个TXT文本文件，输入待转换文本，另存为UTF8编码格式文件，
*  打开U2C.exe小工具，先导入文件文件，然后选择"Convert"进行转换，完成后在同路径下输出
*  同名.c文件。
*/
#define UTF8_RGBLEDDLG     "RGB\xe5\xbd\xa9\xe7\x81\xaf"  //RGB彩灯
#define UTF8_ADCONVER      "AD\xe8\xbd\xac\xe6\x8d\xa2"   //AD转换
#define UTF8_ADRESULT      "AD\xe8\xbd\xac\xe6\x8d\xa2\xe7\xbb\x93\xe6\x9e\x9c:"     //AD转换结果
#define UTF8_VOLTAGE       "\xe7\x94\xb5\xe5\x8e\x8b\xe5\x80\xbc:"//电压值
/*-----------------------------------------------------------------------------------*/


// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// USER START (Optionally insert additional static data)
static uint8_t  rValue,gValue,bValue;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateRGBLED[] = {
  { FRAMEWIN_CreateIndirect, "RGBLED",  0,              0,   0,   480, 854, 0, 0x0, 0 },
  { SLIDER_CreateIndirect,   "rSlider", GUI_ID_SLIDER0, 90, 170, 50, 250, 8, 0x0, 0 },
  { SLIDER_CreateIndirect,   "gSlider", GUI_ID_SLIDER1,  215, 170, 50, 250, 8, 0x0, 0 },
  { SLIDER_CreateIndirect,   "bSlider", GUI_ID_SLIDER2, 330, 170, 50, 250, 8, 0x0, 0 },
  { EDIT_CreateIndirect,     "rEdit",   GUI_ID_EDIT0,   90, 120, 50, 40, 0, 0x3, 0 },
  { EDIT_CreateIndirect,     "gEdit",   GUI_ID_EDIT1,   215, 120, 50, 40, 0, 0x3, 0 },
  { EDIT_CreateIndirect,     "bEdit",   GUI_ID_EDIT2,   330, 120, 50, 40, 0, 0x3, 0 },
  { TEXT_CreateIndirect,     "ADValue", GUI_ID_TEXT0,   220, 660, 140, 40, 0, 0x64, 0 },
  { TEXT_CreateIndirect,     "Voltage", GUI_ID_TEXT1,  220, 720, 140, 40, 0, 0x64, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};


/**
  * @brief  回调函数，RGB应用的回调函数
	* @param  pMsg:包含WM_MESSAGE数据的内容
  * @retval none
  */
static void _cbDialogRGBLED(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  char tempstr[50];
  // USER END

  switch (pMsg->MsgId) {
	case WM_DELETE:
		// USER START (Optionally insert additional code for further widget initialization)
		APP_TRACE_DBG(("RGBLEDDLG delete\n"));
	
		/*关闭RGB灯，重新初始化普通LED*/
		ColorLED_TurnOFF();		
		LED_GPIO_Config();
	
		ICON_Clicked[0] = 0;
		UserApp_Running = 0;
    tpad_flag=0;
		WM_DeleteWindow(hkeypad);
    Rheostat_TurnOFF();
		// USER END
	break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'RGBLED'
    //
		hItem = pMsg->hWin;
    sprintf(tempstr,"%s&%s",UTF8_RGBLEDDLG,UTF8_ADCONVER);
		FRAMEWIN_SetText(hItem,tempstr);
		FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'rEdit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
		EDIT_SetUlongMode(hItem,rValue,0,255);		
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'gEdit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
    EDIT_SetUlongMode(hItem,gValue,0,255);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'bEdit'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2);
    EDIT_SetUlongMode(hItem,bValue,0,255);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
//    EDIT_EnableBlink(hItem,250,1);
		//
		// Initialization of 'rSlider'
		//
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0);
		SLIDER_SetRange(hItem,0,255);
		SLIDER_SetValue(hItem,rValue);
		//
		// Initialization of 'gSlider'
		//
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1);
		SLIDER_SetRange(hItem,0,255);
		SLIDER_SetValue(hItem,gValue);
		//
		// Initialization of 'bSlider'
		//
		hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER2);
		SLIDER_SetRange(hItem,0,255);
		SLIDER_SetValue(hItem,bValue);
    //
    // Initialization of 'ADValue'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_BLACK);
    TEXT_SetBkColor(hItem, GUI_WHITE);
    TEXT_SetText(hItem, "0x0000");
    //
    // Initialization of 'Voltage'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_BLACK);
    TEXT_SetBkColor(hItem, GUI_WHITE);
    TEXT_SetText(hItem, "0");
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_SLIDER0: // Notifications sent by 'rSlider'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        rValue=SLIDER_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0));
				EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0),rValue);
				SetColorValue(rValue,gValue,bValue);
				WM_InvalidateWindow(pMsg->hWin);
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
    case GUI_ID_SLIDER1: // Notifications sent by 'gSlider'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        gValue=SLIDER_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1));
				EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1),gValue);
				SetColorValue(rValue,gValue,bValue);
				WM_InvalidateWindow(pMsg->hWin);
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
    case GUI_ID_SLIDER2: // Notifications sent by 'bSlider'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        bValue=SLIDER_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER2));
				EDIT_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2),bValue);
				SetColorValue(rValue,gValue,bValue);
				WM_InvalidateWindow(pMsg->hWin);
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
    case GUI_ID_EDIT0: // Notifications sent by 'rEdit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				KeyPad_Interface(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0),0);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
				rValue=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
				SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER0),rValue);
				SetColorValue(rValue,gValue,bValue);
        WM_InvalidateWindow(pMsg->hWin);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT1: // Notifications sent by 'gEdit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				KeyPad_Interface(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1),0);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
				gValue=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
				SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER1),gValue);
				SetColorValue(rValue,gValue,bValue);
        WM_InvalidateWindow(pMsg->hWin);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT2: // Notifications sent by 'bEdit'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				KeyPad_Interface(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2),0);
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
				bValue=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2));
				SLIDER_SetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_SLIDER2),bValue);
				SetColorValue(rValue,gValue,bValue);
        WM_InvalidateWindow(pMsg->hWin);
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
		GUI_SetColor(GUI_BLACK);
		GUI_Clear();
    GUI_SetPenSize( 2 );
    GUI_SetColor(GUI_BLUE);  
    GUI_DrawRect(45,20,425,600);
    GUI_DrawRect(45,630,425,790);
    GUI_SetFont(&FONT_XINSONGTI_25);
    GUI_SetColor(GUI_LIGHTBLUE);    
    GUI_DispStringHCenterAt(UTF8_RGBLEDDLG, 480/2,10);
    GUI_DispStringHCenterAt(UTF8_ADCONVER,480/2,620);
    
    GUI_SetColor(GUI_LIGHTGREEN);
    GUI_DispStringAt(UTF8_ADRESULT,75,670);
    GUI_DispStringAt(UTF8_VOLTAGE,75,730);
    GUI_DispStringAt("V",370,730);
		//
		// Draw RGB values
		//
		GUI_SetColor((U32)rValue);
		GUI_FillRect(90, 40, 140, 100);
		GUI_SetColor(((U32)gValue) << 8);
		GUI_FillRect(215,40, 265, 100);
		GUI_SetColor(((U32)bValue) << 16);
		GUI_FillRect(330,40, 380, 100);
		//
		// Draw resulting color
		//
		GUI_SetColor(rValue | (((U32)gValue) << 8) | (((U32)bValue) << 16));
		GUI_FillCircle(480/2,510,75);
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
 /**
  * @brief  点击图标后的动作函数，RGB及ADC应用
  * @param  无
  * @retval 无
  */
void FUN_ICON0Clicked(void)
{
  uint8_t timecount=0;
  WM_HWIN hWin;
  float ADVoltage=0;
  char str[10]={0};
	
	//默认RGB颜色值
	rValue=218;
	gValue=37;
	bValue=196;
	
	APP_TRACE_DBG(("RGBLEDDLG create\n"));
  	
	//初始化RGB彩灯
  ColorLED_Config();
	SetColorValue(rValue,gValue,bValue);

	//初始化ADC
	Rheostat_Init();

  //创建窗口
  hWin=GUI_CreateDialogBox(_aDialogCreateRGBLED, GUI_COUNTOF(_aDialogCreateRGBLED), _cbDialogRGBLED, WM_HBKWIN, 0, 0);
		
  while(UserApp_Running) 
	{	
		//定时更新adc值
    if(timecount==50)
    {
      sprintf(str,"0x%04x",ADC_ConvertedValue);
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),str);
			
      ADVoltage = ADC_ConvertedValue*3.3/4096;
      sprintf(str,"%0.3f",ADVoltage);			
      TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),str);
			
      timecount=0;
    }
    timecount++;
    if(tpad_flag)WM_DeleteWindow(hWin);     
		GUI_Delay(10); 				
	}
	
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
