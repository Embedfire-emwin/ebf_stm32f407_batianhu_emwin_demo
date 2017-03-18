/**
  ******************************************************************************
  * @file    ClockDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   时钟的应用窗口
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
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
// USER START (Optionally insert additional defines)
/*--------------------  (使用U2C.exe小工具生成)  ------------------------------------*/
#define UTF8_CLOCKDLG        "\xe6\x97\xb6\xe9\x92\x9f"//时钟
#define UTF8_TIME            "\xe6\x97\xb6\xe9\x97\xb4"//时间
#define UTF8_DATE            "\xe6\x97\xa5\xe6\x9c\x9f"//日期
/*-----------------------------------------------------------------------------------*/

// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// USER START (Optionally insert additional static data)
typedef struct
{
  uint8_t Date_flag;
  uint8_t Date_day;
  uint8_t Date_month;
  uint8_t Date_year;
  uint8_t Time_flag;
  uint8_t Time_hour;
  uint8_t Time_min;
  uint8_t Time_sec;
}DRV_CLOCK;


DRV_CLOCK drv_clock;
extern RTC_TIME rtc_time;
static WM_HTIMER hTimerTime;  
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateClock[] = {
  { FRAMEWIN_CreateIndirect, "Clock",                  0,     0,   0,   480, 854, 0, 0x0,  0 },
  { TEXT_CreateIndirect,     "Clock",       GUI_ID_TEXT0,     130, 20,  200, 40,  0, 0x64, 0 },
  { TEXT_CreateIndirect,     "Date",        GUI_ID_TEXT1,     130, 50,  200, 25,  0, 0x64, 0 },
  { DROPDOWN_CreateIndirect, "Date_month",  GUI_ID_DROPDOWN0, 125, 140, 120, 25,  0, 0x0,  0 },
  { CHECKBOX_CreateIndirect, "Date_Check",  GUI_ID_CHECK0,    40, 140, 25,  25,  0, 0x0,  0 },
  { CHECKBOX_CreateIndirect, "Time_Check",  GUI_ID_CHECK1,    40, 225, 25,  25,  0, 0x0,  0 },
  { BUTTON_CreateIndirect,   "Apply",       GUI_ID_BUTTON0,   (480-90)/2, 270, 90,  40,  0, 0x0,  0 },
  { EDIT_CreateIndirect,     "Date_day",    GUI_ID_EDIT0,     75, 140, 35,  25,  0, 0x2,  0 },
  { EDIT_CreateIndirect,     "Date_year",   GUI_ID_EDIT1,     260, 140, 60,  25,  0, 0x4,  0 },
  { EDIT_CreateIndirect,     "Time_hour",   GUI_ID_EDIT2,     75, 225, 35,  25,  0, 0x2,  0 },
  { EDIT_CreateIndirect,     "Time_min",    GUI_ID_EDIT3,     145, 225, 35,  25,  0, 0x2,  0 },
  { EDIT_CreateIndirect,     "Time_sec",    GUI_ID_EDIT4,     215, 225, 35,  25,  0, 0x2,  0 },
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
static void _cbDialogClock(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  char tempstr[20];  
  static  CALENDAR_DATE   current_date;
  // USER END

  switch (pMsg->MsgId) {
  case WM_DELETE:
		// USER START (Optionally insert additional code for further widget initialization)
		APP_TRACE_DBG(("ClockDLG delete\n"));
//		Flag_ICON5 = 0;
		UserApp_Running = 0;
    tpad_flag=0;    
    WM_DeleteWindow(hkeypad);
    WM_DeleteTimer(hTimerTime);
		// USER END
	  break;
  case WM_INIT_DIALOG:    
    RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
    RTC_GetDate(RTC_Format_BIN, &rtc_time.RTC_Date);
    drv_clock.Time_flag=1;
    drv_clock.Date_flag=1;
    drv_clock.Time_hour=rtc_time.RTC_Time.RTC_Hours;
    drv_clock.Time_min=rtc_time.RTC_Time.RTC_Minutes;
    drv_clock.Time_sec=rtc_time.RTC_Time.RTC_Seconds;
    drv_clock.Date_day=rtc_time.RTC_Date.RTC_Date;
    drv_clock.Date_month=rtc_time.RTC_Date.RTC_Month;
    drv_clock.Date_year=rtc_time.RTC_Date.RTC_Year;
    /* Unfreeze the RTC DR Register */
    (void)RTC->DR;
    //
    // Initialization of 'Clock'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_CLOCKDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'Clock'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hItem, GUI_FONT_32B_1);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_CYAN);
    sprintf(tempstr,"%02d:%02d:%02d",drv_clock.Time_hour,drv_clock.Time_min,drv_clock.Time_sec);
    TEXT_SetText(hItem, tempstr);
    //
    // Initialization of 'Date'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_BOTTOM);
    TEXT_SetTextColor(hItem, GUI_LIGHTGREEN);
    sprintf(tempstr,"%02d/%02d/20%02d",drv_clock.Date_day,drv_clock.Date_month,drv_clock.Date_year);
    TEXT_SetText(hItem, tempstr);
    //
    // Initialization of 'Date_Check'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CHECK0);
    CHECKBOX_SetState(hItem,drv_clock.Date_flag);
    //
    // Initialization of 'Time_Check'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CHECK1);
    CHECKBOX_SetState(hItem,drv_clock.Time_flag);
    //
    // Initialization of 'Date_month'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0);
    DROPDOWN_SetFont(hItem, GUI_FONT_20_ASCII);
    DROPDOWN_SetListHeight(hItem, 245);
    DROPDOWN_AddString(hItem, "January");
    DROPDOWN_AddString(hItem, "February");
    DROPDOWN_AddString(hItem, "March");
    DROPDOWN_AddString(hItem, "April");
    DROPDOWN_AddString(hItem, "May");
    DROPDOWN_AddString(hItem, "June");
    DROPDOWN_AddString(hItem, "July");
    DROPDOWN_AddString(hItem, "August");
    DROPDOWN_AddString(hItem, "September");
    DROPDOWN_AddString(hItem, "October");
    DROPDOWN_AddString(hItem, "November");
    DROPDOWN_AddString(hItem, "December");
    DROPDOWN_SetSel(hItem,drv_clock.Date_month-1);
    //
    // Initialization of 'Apply'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
    //
    // Initialization of 'Date_day'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetUlongMode(hItem,drv_clock.Date_day,1,31);	
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'Date_year'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
    EDIT_SetUlongMode(hItem,drv_clock.Date_year+2000,2000,2099);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'Time_hour'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2);
    EDIT_SetUlongMode(hItem,drv_clock.Time_hour,0,23);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'Time_min'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3);
    EDIT_SetUlongMode(hItem,drv_clock.Time_min,0,59);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'Time_sec'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4);
    EDIT_SetUlongMode(hItem,drv_clock.Time_sec,0,59);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
//    EDIT_EnableBlink(hItem,250,1);
    // USER START (Optionally insert additional code for further widget initialization)
    CALENDAR_SetDefaultColor(CALENDAR_CI_LABEL, GUI_MAGENTA );

		CALENDAR_SetDefaultSize(CALENDAR_SI_HEADER, 50 );
    CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_X, 60 );
    CALENDAR_SetDefaultSize(CALENDAR_SI_CELL_Y, 40 );
    
    CALENDAR_SetDefaultFont(CALENDAR_FI_CONTENT,GUI_FONT_32B_1 );
    CALENDAR_SetDefaultFont(CALENDAR_FI_HEADER, GUI_FONT_32B_1) ;    
    
    CALENDAR_Create(pMsg->hWin, 30, 360+120, drv_clock.Date_year+2000, drv_clock.Date_month, drv_clock.Date_day, 1, GUI_ID_CALENDAR0, WM_CF_SHOW);
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_DROPDOWN0: // Notifications sent by 'Date_month'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;    
    case GUI_ID_CHECK0: // Notifications sent by 'Date_Check'
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
    case GUI_ID_CHECK1: // Notifications sent by 'Time_Check'
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
    case GUI_ID_BUTTON0: // Notifications sent by 'Apply'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)  
        drv_clock.Date_flag=CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, GUI_ID_CHECK0));
        drv_clock.Time_flag=CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, GUI_ID_CHECK1));      
        if(drv_clock.Date_flag)
        {
          current_date.Year=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
          current_date.Month=DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0))+1;
          current_date.Day=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
          
          rtc_time.RTC_Date.RTC_Year  = current_date.Year - 2000;
          rtc_time.RTC_Date.RTC_Month = current_date.Month;
          rtc_time.RTC_Date.RTC_Date  = current_date.Day;
          rtc_time.RTC_Date.RTC_WeekDay = 0;
          RTC_SetDate(RTC_Format_BIN, &rtc_time.RTC_Date);
          
//          hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CALENDAR0);
//          CALENDAR_SetDate(hItem, &current_date);
//          CALENDAR_SetSel(hItem, &current_date); 
          
          /* Date */  
          sprintf(tempstr,"%02d/%02d/%4d",current_date.Day ,current_date.Month, current_date.Year);
          TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1), tempstr);
        }
        if(drv_clock.Time_flag)
        {
          rtc_time.RTC_Time.RTC_Hours   = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2));
          rtc_time.RTC_Time.RTC_Minutes = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3));
          rtc_time.RTC_Time.RTC_Seconds = EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4));
          rtc_time.RTC_Time.RTC_H12     = RTC_H12_AM;
          RTC_SetTime(RTC_Format_BIN, &rtc_time.RTC_Time);    

          /* TIME */ 
          sprintf(tempstr,"%02d:%02d:%02d",rtc_time.RTC_Time.RTC_Hours,rtc_time.RTC_Time.RTC_Minutes,rtc_time.RTC_Time.RTC_Seconds);
          TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0), tempstr);        
        }
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT0: // Notifications sent by 'Date_day'
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
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT1: // Notifications sent by 'Date_year'
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
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT2: // Notifications sent by 'Time_hour'
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
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_EDIT3: // Notifications sent by 'Time_min'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)        
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        KeyPad_Interface(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3),0);
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
    case GUI_ID_EDIT4: // Notifications sent by 'Time_sec'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)        
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        KeyPad_Interface(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4),0);
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
    GUI_DrawRect(20,90,480-30,350);
  
    GUI_SetColor(GUI_WHITE); 
    GUI_SetFont(GUI_FONT_32B_ASCII);
    GUI_DispCharAt(':',122,220);
    GUI_DispCharAt(':',192,220);
  
    GUI_SetColor(GUI_LIGHTGREEN);
    GUI_SetFont(&FONT_XINSONGTI_25);
    GUI_DispStringHCenterAt(UTF8_DATE,480/2,100); 
    GUI_DispStringHCenterAt(UTF8_TIME,480/2,190);       
    break;
  case WM_TIMER:
    RTC_GetTime(RTC_Format_BIN, &rtc_time.RTC_Time);
    RTC_GetDate(RTC_Format_BIN, &rtc_time.RTC_Date);
    drv_clock.Time_hour=rtc_time.RTC_Time.RTC_Hours;
    drv_clock.Time_min=rtc_time.RTC_Time.RTC_Minutes;
    drv_clock.Time_sec=rtc_time.RTC_Time.RTC_Seconds;
    drv_clock.Date_day=rtc_time.RTC_Date.RTC_Date;
    drv_clock.Date_month=rtc_time.RTC_Date.RTC_Month;
    drv_clock.Date_year=rtc_time.RTC_Date.RTC_Year;
    /* Unfreeze the RTC DR Register */
    (void)RTC->DR;
    sprintf(tempstr,"%02d:%02d:%02d",drv_clock.Time_hour,drv_clock.Time_min,drv_clock.Time_sec);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0), tempstr);
  
    sprintf(tempstr,"%02d/%02d/20%02d",drv_clock.Date_day,drv_clock.Date_month,drv_clock.Date_year);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1), tempstr);
  
//    if((current_date.Day!=drv_clock.Date_day)|| \
//        (current_date.Month!=drv_clock.Date_month)|| \
//        (current_date.Year!=(drv_clock.Date_year+2000)))
//    {
//      current_date.Day=drv_clock.Date_day;
//      current_date.Month=drv_clock.Date_month;
//      current_date.Year=drv_clock.Date_year+2000;
//      hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_CALENDAR0);     
//      CALENDAR_SetDate(hItem, &current_date);
//      CALENDAR_SetSel(hItem, &current_date);
//    }
  
    WM_RestartTimer(pMsg->Data.v, 0);
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
*       CreateClock
*/
void FUN_ICON5Clicked(void) 
{
  WM_HWIN hWin;
  
	APP_TRACE_DBG(("ClockDLG create\n")); 
 
	if(bsp_result&BSP_RTC)
  {
    if(RTC_CLK_Config() == ERROR)  
      bsp_result |=BSP_RTC;
    else
		{
      bsp_result &=~BSP_RTC;		
		}
  }
  hWin = GUI_CreateDialogBox(_aDialogCreateClock, GUI_COUNTOF(_aDialogCreateClock), _cbDialogClock, WM_HBKWIN, 0, 0);
  hTimerTime=WM_CreateTimer(WM_GetClientWindow(hWin), 0, 1000, 0);  
  while(UserApp_Running)
  {
    if(tpad_flag)WM_DeleteWindow(hWin);
    GUI_Delay(10);
  }
    
}

// USER START (Optionally insert additional public code)
// USER END
/*************************** End of file ****************************/
