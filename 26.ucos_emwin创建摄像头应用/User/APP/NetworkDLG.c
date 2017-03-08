/**
  ******************************************************************************
  * @file    NetworkDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   网络的应用窗口
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
#include "netconf.h"
#include "common.h"
#include "tcp_echoclient.h"
#include "tcp_echoserver.h"
#include "udp_echoclient.h"
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
// USER START (Optionally insert additional defines)
/*--------------------  (使用U2C.exe小工具生成)  ------------------------------------*/
#define UTF8_NETWORKDLG        "\xe7\xbd\x91\xe7\xbb\x9c\xe9\x80\x9a\xe4\xbf\xa1"//网络通信
#define UTF8_PROTOCOL          "\xe9\x80\x9a\xe4\xbf\xa1\xe5\x8d\x8f\xe8\xae\xae:"//通信协议
#define UTF8_LOCAL             "\xe6\x9c\xac\xe5\x9c\xb0"//本地
#define UTF8_ADDRESS           "\xe5\x9c\xb0\xe5\x9d\x80"//地址
#define UTF8_PORT              "\xe7\xab\xaf\xe5\x8f\xa3:"//端口
#define UTF8_SERVER            "\xe6\x9c\x8d\xe5\x8a\xa1\xe5\x99\xa8"//服务器
#define UTF8_REMOTE            "\xe8\xbf\x9c\xe7\xab\xaf"//远端
#define UTF8_DATA              "\xe6\x95\xb0\xe6\x8d\xae"//数据
#define UTF8_RECEIVE           "\xe6\x8e\xa5\xe6\x94\xb6"//接收
#define UTF8_SEND              "\xe5\x8f\x91\xe9\x80\x81"//发送
#define UTF8_DISCONNECT        "\xe6\x9c\xaa\xe8\xbf\x9e\xe6\x8e\xa5"//未连接
#define UTF8_CONNECT           "\xe9\x80\x9a\xe4\xbf\xa1\xe4\xb8\xad"//通信中
#define UTF8_NETWORKERROR      "\xe7\xbd\x91\xe7\xbb\x9c\xe9\x94\x99\xe8\xaf\xaf"//网络错误
#define UTF8_NETWORKERROR1     "\xe8\xaf\xb7\xe7\xa1\xae\xe4\xbf\x9d\xe6\x8f\x92\xe5\x85\xa5\xe7\xbd\x91\xe7\xba\xbf"//请确保接入网线
/*-----------------------------------------------------------------------------------*/
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// USER START (Optionally insert additional static data) 
DRV_NETWORK drv_network={0};

extern struct netif gnetif;
extern __IO uint8_t EthLinkStatus;
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */

// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateNetwork[] = {
  { FRAMEWIN_CreateIndirect,  "Network",                     0,  0,   0,   480, 854, 0, 0x0,  0 },
  { DROPDOWN_CreateIndirect,  "NetTpye",      GUI_ID_DROPDOWN0,  10,  40,  130, 27,  0, 0x0,  0 },
  { BUTTON_CreateIndirect,    "Connect",      GUI_ID_BUTTON0,    170, 37,  90,  32,  0, 0x0,  0 },  
  { EDIT_CreateIndirect,      "REMOTEIPAdd1", GUI_ID_EDIT0,      10,  185, 45,  25,  0, 0x3,  0 },
  { EDIT_CreateIndirect,      "REMOTEIPAdd2", GUI_ID_EDIT1,      70,  185, 45,  25,  0, 0x3,  0 },
  { EDIT_CreateIndirect,      "REMOTEIPAdd3", GUI_ID_EDIT2,      130, 185, 45,  25,  0, 0x3,  0 },
  { EDIT_CreateIndirect,      "REMOTEIPAdd4", GUI_ID_EDIT3,      190, 185, 45,  25,  0, 0x3,  0 },
  { EDIT_CreateIndirect,      "REMOTEPort",   GUI_ID_EDIT4,      260, 185, 70,  25,  0, 0x4,  0 },
  { MULTIEDIT_CreateIndirect, "SendBuf",      GUI_ID_MULTIEDIT0, 10, 540,  480-30, 250, 0, 0x0,  0 },
  { MULTIEDIT_CreateIndirect, "ReceiveBuf",   GUI_ID_MULTIEDIT1, 10,  260, 480-30, 240, 0, 0x0,  0 },
  { BUTTON_CreateIndirect,    "Clear",        GUI_ID_BUTTON2,    200, 225, 90,  32,  0, 0x0,  0 },
  { BUTTON_CreateIndirect,    "Send",         GUI_ID_BUTTON1,    200, 505,  90,  32,  0, 0x0,  0 },
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
  * @brief  通用定时器3中断初始化
  * @param  period : 自动重装值。
  * @param  prescaler : 时钟预分频数
  * @retval 无
  * @note   定时器溢出时间计算方法:Tout=((period+1)*(prescaler+1))/Ft us.
  *          Ft=定时器工作频率,为SystemCoreClock/2=90,单位:Mhz
  */
static void TIM3_Config(uint16_t period,uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=prescaler;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=period;   //自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  定时器3中断服务函数
  * @param  无
  * @retval 无
  */
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		LocalTime+=10;//10ms增量
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogNetwork(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  char tempstr[30];
  // USER END

  switch (pMsg->MsgId) {
  case WM_DELETE:
		// USER START (Optionally insert additional code for further widget initialization)
		APP_TRACE_DBG(("NetworkDLG delete\n"));
//		Flag_ICON6 = 0;
		UserApp_Running = 0;
    tpad_flag=0;    
    WM_DeleteWindow(hkeypad);
  	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
  	TIM_Cmd(TIM3,DISABLE);
		// USER END
	  break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Network'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_NETWORKDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'NetTpye'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0);
    DROPDOWN_AddString(hItem, "Tcp Client");
    DROPDOWN_AddString(hItem, "Tcp Server");
    DROPDOWN_AddString(hItem, "UDP");
    DROPDOWN_SetFont(hItem, GUI_FONT_20B_ASCII);
    DROPDOWN_SetListHeight(hItem, 85);
    DROPDOWN_SetItemSpacing(hItem,7);
    DROPDOWN_SetTextAlign(hItem,GUI_TA_LEFT | GUI_TA_VCENTER);    
    //
    // Initialization of 'REMOTEIPAdd1'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetUlongMode(hItem,drv_network.net_remote_ip1,0,255);	
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'REMOTEIPAdd2'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetUlongMode(hItem,drv_network.net_remote_ip2,0,255);	
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'REMOTEIPAdd3'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetUlongMode(hItem,drv_network.net_remote_ip3,0,255);	
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'REMOTEIPAdd4'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3);
    EDIT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetUlongMode(hItem,drv_network.net_remote_ip4,0,255);	
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'REMOTEPort'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4);
    EDIT_SetFont(hItem, GUI_FONT_24B_ASCII);
    EDIT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    EDIT_SetUlongMode(hItem,drv_network.net_remote_port,0,9999);	
//    EDIT_EnableBlink(hItem,250,1);
    //
    // Initialization of 'SendBuf'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0);
    MULTIEDIT_SetText(hItem, "Hello World");
    MULTIEDIT_SetFont(hItem, &FONT_XINSONGTI_25);
//		MULTIEDIT_EnableBlink(hItem,250,1);
		MULTIEDIT_SetAutoScrollV(hItem,1);
		MULTIEDIT_SetAutoScrollH(hItem,1);     
    //
    // Initialization of 'ReceiveBuf'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1);
    MULTIEDIT_SetText(hItem, "");
    MULTIEDIT_SetFont(hItem, &FONT_XINSONGTI_25);
		MULTIEDIT_SetAutoScrollV(hItem,1);
		MULTIEDIT_SetAutoScrollH(hItem,1); 
    MULTIEDIT_SetMaxNumChars(hItem,2048);
    MULTIEDIT_SetReadOnly(hItem,1);
    //
    // Initialization of 'Connect'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, &FONT_XINSONGTI_25);
    BUTTON_SetText(hItem,UTF8_DISCONNECT);
    //
    // Initialization of 'Send'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
    BUTTON_SetFont(hItem, &FONT_XINSONGTI_25);
    BUTTON_SetText(hItem,UTF8_SEND);
    //
    // Initialization of 'Clear'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON2);
    BUTTON_SetFont(hItem, GUI_FONT_16B_ASCII);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_DROPDOWN0: // Notifications sent by 'NetTpye'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        drv_network.net_type=DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0));
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
    case GUI_ID_EDIT0: // Notifications sent by 'REMOTEIPAdd1'
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
    case GUI_ID_EDIT1: // Notifications sent by 'REMOTEIPAdd2'
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
    case GUI_ID_EDIT2: // Notifications sent by 'REMOTEIPAdd3'
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
    case GUI_ID_EDIT3: // Notifications sent by 'REMOTEIPAdd4'
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
    case GUI_ID_EDIT4: // Notifications sent by 'REMOTEPort'
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
    case GUI_ID_MULTIEDIT0: // Notifications sent by 'SendBuf'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        KeyPad_Interface(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),1);
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
    case GUI_ID_MULTIEDIT1: // Notifications sent by 'ReceiveBuf'
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
        if(MULTIEDIT_GetTextSize(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1))>2000)
            MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1),"");
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON0: // Notifications sent by 'Connect'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)        
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if((bsp_result&BSP_NETWORK)||EthLinkStatus)
        {
          break;
        }
        if(drv_network.net_connect==0)
        {
          uint8_t connectflag;
          
          drv_network.net_remote_ip1=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT0));
          drv_network.net_remote_ip2=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT1));
          drv_network.net_remote_ip3=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT2));
          drv_network.net_remote_ip4=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT3));
          drv_network.net_remote_port=EDIT_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_EDIT4));
          drv_network.net_type=DROPDOWN_GetSel(WM_GetDialogItem(pMsg->hWin, GUI_ID_DROPDOWN0));          
          switch(drv_network.net_type)
          {
            case 0:
              /*connect to tcp server */ 
              connectflag=tcp_echoclient_connect(drv_network);
              break;
            case 1:
              /*create tcp server */ 
              connectflag=tcp_echoserver_init(drv_network);
              break;
            case 2:
              /* Connect to tcp server */ 
				      connectflag=udp_echoclient_connect(drv_network);		
              break;            
          }
          if(connectflag==0)
          {
            drv_network.net_connect=1; 
            BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0),UTF8_CONNECT);
          }      
        }
        else
        {
          BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0),UTF8_DISCONNECT);
          switch(drv_network.net_type)
          {
            case 0:
              tcp_echoclient_disconnect();
              break;
            case 1:
              tcp_echoserver_close();
              break;
            case 2:
				      udp_echoclient_disconnect();	
              break;            
          }
          drv_network.net_connect=0;
        }
        WM_InvalidateWindow(pMsg->hWin);
        WM_Paint(pMsg->hWin);   
        WM_DeleteWindow(hkeypad);        
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'Send'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        WM_DeleteWindow(hkeypad);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        if(drv_network.net_connect==1)          
        {
          MULTIEDIT_GetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),(char *)comdata,1024);
          com_utf82gbk((char *)comdata,(char *)comdata);
          switch(drv_network.net_type)
          {
            case 0:
              network_tcpclient_send((char *)comdata);
              break;
            case 1:
              network_tcpserver_send((char *)comdata);
              break;
            case 2:
              udp_echoclient_send((char *)comdata);
              break;            
          }
        }
        MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0), "");
        WM_DeleteWindow(hkeypad);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON2: // Notifications sent by 'Clear'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0), "");
        MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1), "");
        WM_DeleteWindow(hkeypad);
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
   GUI_SetColor(GUI_LIGHTGREEN);
   GUI_SetFont(&FONT_XINSONGTI_25);  
   GUI_DispStringAt(UTF8_PROTOCOL, 10,10); 
   sprintf(tempstr,"%s%s:",UTF8_DATA,UTF8_SEND);
   GUI_DispStringAt(tempstr,10,510); 
   sprintf(tempstr,"%s%s:",UTF8_DATA,UTF8_RECEIVE);
   GUI_DispStringAt(tempstr,10,230); 
   GUI_DispStringAt(UTF8_PORT,260,155); 
 
   switch(drv_network.net_type)
   {
     case 0:
       sprintf(tempstr,"%sIP%s:",UTF8_SERVER,UTF8_ADDRESS);
       GUI_DispStringAt(tempstr,10,155); 
       break;
     case 1:
     case 2:
       sprintf(tempstr,"%sIP%s:",UTF8_REMOTE,UTF8_ADDRESS);
       GUI_DispStringAt(tempstr,10,155);   
       break;
   } 
   sprintf(tempstr,"%sIP%s&%s",UTF8_LOCAL,UTF8_ADDRESS,UTF8_PORT);
   GUI_DispStringAt(tempstr,10,85);    
   
   GUI_SetFont(GUI_FONT_24B_ASCII); 
   GUI_SetColor(GUI_WHITE);
   GUI_DispCharAt('.',60,185);
   GUI_DispCharAt('.',120,185);
   GUI_DispCharAt('.',180,185);
   GUI_DispStringAt(":",243,185); 
   GUI_SetColor(GUI_LIGHTGRAY);
   sprintf(tempstr,"[ %d.%d.%d.%d : %d ]",drv_network.net_local_ip1,drv_network.net_local_ip2,\
                                       drv_network.net_local_ip3,drv_network.net_local_ip4,\
                                       drv_network.net_local_port);
   GUI_DispStringAt(tempstr,20,115); 
   
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
*       CreateNetwork
*/
void FUN_ICON6Clicked(void) 
{
  static uint8_t startflag=0;
  OS_ERR  err;
  WM_HWIN hWin;
  TEXT_Handle htext;
  APP_TRACE_DBG(("NetworkDLG create\n"));

  hWin = GUI_CreateDialogBox(_aDialogCreateNetwork, GUI_COUNTOF(_aDialogCreateNetwork), _cbDialogNetwork, WM_HBKWIN, 0, 0);
  drv_network.hWin=hWin;
  htext=TEXT_CreateEx(90,270,300,50,hWin,WM_CF_SHOW|WM_CF_STAYONTOP,TEXT_CF_HCENTER|GUI_TA_VCENTER,GUI_ID_TEXT9,"Initialize Network...");
  TEXT_SetFont(htext, GUI_FONT_32B_ASCII);
  TEXT_SetBkColor(htext,GUI_BLACK);
  TEXT_SetTextColor(htext, GUI_WHITE);
  GUI_Delay(100);
  
  OSSchedLock(&err);
  if(startflag==0)
  {
    /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
    if(ETH_BSP_Config()==1)
    {
      bsp_result |=BSP_NETWORK;
    }
    else
    {
      bsp_result &=~ BSP_NETWORK;        
    }
    startflag=1;
  }
  if((drv_network.net_init==0)&&((bsp_result&BSP_NETWORK)==0))
  {     
    /* Initilaize the LwIP stack */
    LwIP_Init(); 

    drv_network.net_local_ip1=(uint8_t)(gnetif.ip_addr.addr&0xFF);
    drv_network.net_local_ip2=(uint8_t)((gnetif.ip_addr.addr>>8)&0xFF);
    drv_network.net_local_ip3=(uint8_t)((gnetif.ip_addr.addr>>16)&0xFF);
    drv_network.net_local_ip4=(uint8_t)((gnetif.ip_addr.addr>>24)&0xFF);
    drv_network.net_local_port=LOCAL_PORT;
    
    drv_network.net_remote_ip1=DEST_IP_ADDR0;
    drv_network.net_remote_ip2=DEST_IP_ADDR1;
    drv_network.net_remote_ip3=DEST_IP_ADDR2;
    drv_network.net_remote_ip4=DEST_IP_ADDR3;
    drv_network.net_remote_port=DEST_PORT;     
    
    drv_network.net_init=1;
  }
  OSSchedUnlock(&err);    
  WM_DeleteWindow(WM_GetClientWindow(htext));
  if(bsp_result&BSP_NETWORK)
  {		
    char str[30];
    if(startflag==2)
    {
      /* Configure ethernet (GPIOs, clocks, MAC, DMA) */
      if(ETH_BSP_Config()==1)  
      {
        bsp_result |= BSP_NETWORK;
        sprintf(str,UTF8_NETWORKERROR1);  
      }
      else
      {
        bsp_result &=~ BSP_NETWORK;    
        sprintf(str,"< Network must be restart  >"); 
      }
    }
    else
    {
      sprintf(str,UTF8_NETWORKERROR1);  
    }
    startflag=2;
    ErrorDialog(hWin,UTF8_NETWORKERROR,str);
    while(1)
    {
      if(tpad_flag)WM_DeleteWindow(hWin);
       if(!UserApp_Running)return;
       GUI_Delay(10);
    }
  }
  DROPDOWN_SetSel(WM_GetDialogItem(hWin, GUI_ID_DROPDOWN0),drv_network.net_type);
  EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT0),drv_network.net_remote_ip1);
  EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT1),drv_network.net_remote_ip2);
  EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT2),drv_network.net_remote_ip3);
  EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT3),drv_network.net_remote_ip4);
  EDIT_SetValue(WM_GetDialogItem(hWin, GUI_ID_EDIT4),drv_network.net_remote_port);
  WM_InvalidateWindow(hWin);
  
  drv_network.net_connect=0;
  drv_network.net_type=0; 
  TIM3_Config(999,899);//10ms定时器 
  LocalTime=0;
  TIM_SetCounter(TIM3,0);
  EthLinkStatus=0;
  while(UserApp_Running)
  {
    if(EthLinkStatus)
    {
      ErrorDialog(hWin,UTF8_NETWORKERROR,UTF8_NETWORKERROR1);
      while(1)
      {
        if(tpad_flag)WM_DeleteWindow(hWin);
         if(!UserApp_Running)return;
         GUI_Delay(10);
      }
    }
    /* check if any packet received */
    if (ETH_CheckFrameReceived())
    { 
      /* process received ethernet packet */
      LwIP_Pkt_Handle();
    }
    /* handle periodic timers for LwIP */
    LwIP_Periodic_Handle(LocalTime);
    if(tpad_flag)WM_DeleteWindow(hWin);
    GUI_Delay(1);//WM_Exec();//
  }
}

// USER START (Optionally insert additional public code)
// USER END
/*************************** End of file ****************************/
