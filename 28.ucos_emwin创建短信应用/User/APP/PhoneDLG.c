/**
  ******************************************************************************
  * @file    PhoneDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   电话的应用窗口
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
//#include "app.h"
#include "DIALOG.h"
#include "./Bsp/gsm_gprs/bsp_gsm_gprs.h"
// USER END
/*
**************************************************************************************
*                                         宏定义 
**************************************************************************************
*/ 
// USER START (Optionally insert additional defines)
/*--------------------  (使用U2C.exe小工具生成)  ------------------------------------*/
#define UTF8_PHONEDLG        "\xe7\x94\xb5\xe8\xaf\x9d"//电话
#define UTF8_SPEAKING        "\xe9\x80\x9a\xe8\xaf\x9d\xe4\xb8\xad..."//通话中
#define UTF8_RING            "\xe6\x9d\xa5\xe7\x94\xb5\xe5\x91\xbc\xe5\x8f\xab..."//呼叫
#define UTF8_CALL            "\xe6\x8b\xa8\xe5\x8f\xb7..."//拨号    
#define UTF8_GSMERROR        "GSM\xe6\xa8\xa1\xe5\x9d\x97\xe9\x80\x9a\xe4\xbf\xa1\xe5\xa4\xb1\xe8\xb4\xa5"//GSM模块通信失败
#define UTF8_GSMERROR1       "\xe8\xaf\xb7\xe6\xa3\x80\xe6\x9f\xa5\xe6\x8e\xa5\xe7\xba\xbf"//请检查接线       
/*-----------------------------------------------------------------------------------*/

#define ID_BUTTON             (GUI_ID_USER + 0)

#define BUTTON_WHITH          100
#define BUTTON_HIGHT          80
#define BUTTON_SpaceX         (10+BUTTON_WHITH) 
#define BUTTON_SpaceY         (10+BUTTON_HIGHT)
#define TEXTLENGTH            25
// USER END
/*
**************************************************************************************
*                                      变量
**************************************************************************************
*/ 
extern GUI_CONST_STORAGE GUI_BITMAP bmhangon;
extern GUI_CONST_STORAGE GUI_BITMAP bmhangoff;
extern GUI_CONST_STORAGE GUI_BITMAP bmdelete;

static WM_HWIN hKey,hBack,hText;

static uint8_t 		s_Key;
static char 		  text[TEXTLENGTH];
static uint8_t 		IsCall=0;//1:calling  0:waiting
static uint8_t		textoff=0;

typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;   /* 按钮对应的字符 */
} BUTTON;

static const BUTTON ButtonData[] = 
{

	/* 第1排按钮 */
	{ 5,                   3,                   BUTTON_WHITH, 		BUTTON_HIGHT, "1"  },
	{ 5+BUTTON_SpaceX,     3,                   BUTTON_WHITH, 		BUTTON_HIGHT, "2"  },
	{ 5+BUTTON_SpaceX*2,   3,                   BUTTON_WHITH, 		BUTTON_HIGHT, "3"  },
	/* 第2排按钮 */
	{ 5,                   3+BUTTON_SpaceY,     BUTTON_WHITH, 		BUTTON_HIGHT, "4"  },	
	{ 5+BUTTON_SpaceX,     3+BUTTON_SpaceY,     BUTTON_WHITH, 		BUTTON_HIGHT, "5"  },
	{ 5+BUTTON_SpaceX*2,   3+BUTTON_SpaceY,     BUTTON_WHITH, 		BUTTON_HIGHT, "6"  },
	/* 第3排按钮 */
	{ 5,                   3+BUTTON_SpaceY*2,  	BUTTON_WHITH, 		BUTTON_HIGHT, "7"  },
	{ 5+BUTTON_SpaceX,     3+BUTTON_SpaceY*2,  	BUTTON_WHITH, 		BUTTON_HIGHT, "8"  },	
	{ 5+BUTTON_SpaceX*2,   3+BUTTON_SpaceY*2,  	BUTTON_WHITH, 		BUTTON_HIGHT, "9"  },
	/* 第4排按钮 */
	{ 5,                   3+BUTTON_SpaceY*3,  	BUTTON_WHITH, 		BUTTON_HIGHT, "*"  },
	{ 5+BUTTON_SpaceX,     3+BUTTON_SpaceY*3,  	BUTTON_WHITH, 		BUTTON_HIGHT, "0"  },
	{ 5+BUTTON_SpaceX*2,   3+BUTTON_SpaceY*3,  	BUTTON_WHITH, 		BUTTON_HIGHT, "#"  }
};

/***********************************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreatePhone[] = {
	{ FRAMEWIN_CreateIndirect, "Phone",                0, 0,   0,   480, 854, 0, 0x0,  0 },
	{ TEXT_CreateIndirect,     "number",    GUI_ID_TEXT0, 90, 140,  280, 40,  0, 0x64, 0 },
	{ TEXT_CreateIndirect,     "calling",   GUI_ID_TEXT1, 130, 250, 200, 40,  0, 0x64, 0 },
	{ BUTTON_CreateIndirect,   "back",    GUI_ID_BUTTON0, 395, 145,  24,  24,  0, 0x0,  0 },
	{ BUTTON_CreateIndirect,   "hangon",  GUI_ID_BUTTON1, 145, 600, 48,  48,  0, 0x0,  0 },
	{ BUTTON_CreateIndirect,   "hangoff", GUI_ID_BUTTON2, 275, 600, 48,  48,  0, 0x0,  0 },
	{ BUTTON_CreateIndirect,   "call",    GUI_ID_BUTTON3, 210, 600, 48,  48,  0, 0x0,  0 },
	// USER START (Optionally insert additional widgets)
	// USER END
};
/*
*************************************************************************************
*	函 数 名: _cbKeyPad
*	功能说明: 回调函数
*	形    参：pMsg  指针参数
*	返 回 值: 无
*************************************************************************************
*/
static void _cbDialogPhone(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int        Id;
	int        NCode;
	switch (pMsg->MsgId) 
	{
  case WM_DELETE:
    APP_TRACE_DBG(("PhoneDLG delete\n"));
//    Flag_ICON8 = 0;
    textoff=0;
    text[0]='\0';
    IsCall=0;  
		
	  GSM_USART_Stop();
   	UserApp_Running = 0;
    tpad_flag=0;
    break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Phone'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_PHONEDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'number'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    hText=hItem;
    TEXT_SetTextColor(hItem, GUI_BLACK);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetText(hItem, "112");
    //
    // Initialization of 'calling'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetTextColor(hItem, GUI_BLACK);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, &FONT_XINSONGTI_25);
    TEXT_SetText(hItem, UTF8_CALL);
    WM_HideWindow(hItem);
    //
    // Initialization of 'back'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    hBack=hItem;
    BUTTON_SetBitmap(hItem,BUTTON_BI_UNPRESSED,&bmdelete);			
    BUTTON_SetText(hItem,"");
    //WM_HideWindow(hBack);
    //
    // Initialization of 'hangon'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
    BUTTON_SetBitmap(hItem,BUTTON_BI_UNPRESSED,&bmhangon);
    BUTTON_SetText(hItem,"");
    WM_BringToTop(hItem);
    WM_HideWindow(hItem);
    //
    // Initialization of 'hangoff'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
    BUTTON_SetBitmap(hItem,BUTTON_BI_UNPRESSED,&bmhangoff);
    BUTTON_SetText(hItem,"");
    WM_BringToTop(hItem);
    WM_HideWindow(hItem);
    //
    // Initialization of 'call'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3);
    BUTTON_SetBitmap(hItem,BUTTON_BI_UNPRESSED,&bmhangon);
    BUTTON_SetText(hItem,"");
    WM_BringToTop(hItem);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
      case GUI_ID_BUTTON0: // Notifications sent by 'back'
        switch(NCode) {
          case WM_NOTIFICATION_CLICKED:
          // USER START (Optionally insert code for reacting on notification message)
          textoff--;
          if(textoff==255)
          {
            textoff=0;
          }
          // USER END
          break;
          case WM_NOTIFICATION_RELEASED:
          // USER START (Optionally insert code for reacting on notification message)
          text[textoff]='\0';
          TEXT_SetText(hText,text);
          //printf("text:%s\n",text);
          if(textoff==0)WM_HideWindow(hBack);
          // USER END
          break;
          // USER START (Optionally insert additional code for further notification handling)
          // USER END
        }
        break;
      case GUI_ID_BUTTON1: // Notifications sent by 'hangon'
        switch(NCode) {
          case WM_NOTIFICATION_CLICKED:
          // USER START (Optionally insert code for reacting on notification message)							
          // USER END
          break;
          case WM_NOTIFICATION_RELEASED:
          // USER START (Optionally insert code for reacting on notification message)
          IsCall=1;							
          GSM_HANGON();
          TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1), UTF8_SPEAKING);
          GSM_CLEAN_RX();//清除接收缓存
          // USER END
          break;
          // USER START (Optionally insert additional code for further notification handling)
          // USER END
        }
        break;
      case GUI_ID_BUTTON2: // Notifications sent by 'hangoff'
        switch(NCode) {
          case WM_NOTIFICATION_CLICKED:
          // USER START (Optionally insert code for reacting on notification message)										
          // USER END
          break;
          case WM_NOTIFICATION_RELEASED:
          // USER START (Optionally insert code for reacting on notification message)
          /*挂电话*/
          GSM_HANGOFF();	
          IsCall=0;
          
          WM_ShowWindow(hKey);
          TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1), UTF8_CALL);
          WM_HideWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1));
          textoff=0;
          text[0]='\0';
          TEXT_SetText(hText,text);						
          WM_HideWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1));
          WM_HideWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2));
          WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3));
          GSM_CLEAN_RX();//清除接收缓存
          // USER END
          break;
          // USER START (Optionally insert additional code for further notification handling)
          // USER END
        }
        break;
      case GUI_ID_BUTTON3: // Notifications sent by 'call'
        switch(NCode) {
          case WM_NOTIFICATION_CLICKED:
          // USER START (Optionally insert code for reacting on notification message)							
          // USER END
          break;
          case WM_NOTIFICATION_RELEASED:
          // USER START (Optionally insert code for reacting on notification message)
          if(IsCall==0)
          {
            //拨打电话号码
            gsm_call(text);
            BUTTON_SetBitmap(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3),BUTTON_BI_UNPRESSED,&bmhangoff);
            WM_HideWindow(hKey);
            WM_HideWindow(hBack);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1), UTF8_SPEAKING);
            WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1));			
            IsCall=1;
          }else	if(IsCall==1)
          {
            /*挂电话*/
            GSM_HANGOFF();	
            BUTTON_SetBitmap(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON3),BUTTON_BI_UNPRESSED,&bmhangon);
            WM_ShowWindow(hKey);
            WM_HideWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1));
            textoff=0;
            text[0]='\0';
            TEXT_SetText(hText,text);
            IsCall=0;
          }
          GSM_CLEAN_RX();//清除接收缓存
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
  /* 绘制背景 */	
  case WM_PAINT:	
		GUI_SetBkColor(APPBKCOLOR);
    GUI_Clear();
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(30,120,440,670);
    break;
  default:
  WM_DefaultProc(pMsg);
  break;
	}
}

static void _cbKeypad9(WM_MESSAGE * pMsg)
{
	WM_HWIN hButton;
	uint8_t i;
	int        Id;
	int        NCode;
	switch (pMsg->MsgId) 
	{
		case WM_CREATE:
			// USER START (Optionally insert additional code for further widget initialization)
			/* 创建所需的按钮 */
			for (i = 0; i < GUI_COUNTOF(ButtonData); i++) 
			{
				hButton = BUTTON_CreateEx(ButtonData[i].xPos, ButtonData[i].yPos, ButtonData[i].xSize, ButtonData[i].ySize, 
				WM_GetClientWindow(pMsg->hWin), WM_CF_SHOW, 0, ID_BUTTON + i);
				BUTTON_SetFont(hButton, &GUI_Font32B_ASCII);
				BUTTON_SetText(hButton, ButtonData[i].acLabel);
				BUTTON_SetTextAlign(hButton,GUI_TA_HCENTER|GUI_TA_VCENTER);
				BUTTON_SetFocussable(hButton, 0);
			}
			// USER END
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) 
			{
				case WM_NOTIFICATION_CLICKED:
					// USER START (Optionally insert code for reacting on notification message)
					LED1_ON;
				#ifdef USERBEEP
					BEEP_ON;
				#endif
					WM_ShowWindow(hBack);
					// USER END
				break;
				case WM_NOTIFICATION_RELEASED:					
					// USER START (Optionally insert code for reacting on notification message)
					LED1_OFF;
				#ifdef USERBEEP
					BEEP_OFF;
				#endif
					s_Key = ButtonData[Id - ID_BUTTON].acLabel[0];
					text[textoff]=s_Key;
					text[textoff+1]='\0';
					TEXT_SetText(hText,text);
					//printf("text:%s\n",text);
					textoff++;
					// USER END
				break;
			    // USER START (Optionally insert additional code for further notification handling)
			    // USER END
			}
			break;
		/* 绘制背景 */	
		case WM_PAINT:	
			GUI_SetBkColor(GUI_WHITE);
			GUI_Clear();
			break;
		default:
		WM_DefaultProc(pMsg);
		break;
	}
}
/*
*********************************************************************************************************
*	函 数 名: MainAPP
*	功能说明: GUI主函数 
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void FUN_ICON8Clicked(void)
{
	WM_HWIN hWin;
	char num[20]={0};
	uint8_t timecount=0;
	IsCall=0;
	sprintf(text,"112");
	APP_TRACE_DBG(("PhoneDLG create\n"));
	hWin=GUI_CreateDialogBox(_aDialogCreatePhone, GUI_COUNTOF(_aDialogCreatePhone), _cbDialogPhone, WM_HBKWIN, 0, 0);
	hKey=WM_CreateWindowAsChild(75,240,330,380,hWin,WM_CF_SHOW,_cbKeypad9,0);
	WM_BringToTop(hKey);
	GUI_Delay(100);
	/* 初始化并检测模块 */
	if(gsm_init()!= GSM_TRUE)
	{    
    ErrorDialog(hWin,UTF8_GSMERROR,UTF8_GSMERROR1);
    while(1)
    {
      if(tpad_flag)WM_DeleteWindow(hWin);
       if(!UserApp_Running)return;
       GUI_Delay(10);
    }
	}
	GSM_CLEAN_RX();//清除接收缓存

	GSM_TX("AT+CLIP=1\r");
	timecount=0;
	while(UserApp_Running)
	{
		if(timecount>50)
		{
			if(IsRing(num)== GSM_TRUE)
			{
				BEEP_ON;
				TEXT_SetText(hText,num);
				WM_HideWindow(hKey);
				WM_HideWindow(hBack);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),UTF8_RING);
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON1));
				WM_ShowWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON2));
				WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_BUTTON3));				
			}
			if(IsCall&&(IsNOCARRIER()==GSM_TRUE))
      {
        BUTTON_SetBitmap(WM_GetDialogItem(hWin, GUI_ID_BUTTON3),BUTTON_BI_UNPRESSED,&bmhangon);
        WM_ShowWindow(hKey);
        WM_HideWindow(WM_GetDialogItem(hWin, GUI_ID_TEXT1));
        textoff=0;
        text[0]='\0';
        TEXT_SetText(hText,text);
        IsCall=0;
      }
			BEEP_OFF;
      GSM_CLEAN_RX();//清除接收缓存
			timecount=0;
		}
		timecount++;
		if(tpad_flag)WM_DeleteWindow(hWin);
		GUI_Delay(10);
	}
}
