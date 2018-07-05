/**
  ******************************************************************************
  * @file    CalculatorDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   计算器的的应用窗口
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
//#include "app.h"
#include "DIALOG.h"
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
#define UTF8_CALCULATORDLG         "\xe8\xae\xa1\xe7\xae\x97\xe5\x99\xa8"//计算器
/*-----------------------------------------------------------------------------------*/

#define ID_CALBUTTON              (GUI_ID_USER + 50)
#define CALBUTTON_WHITH           65
#define CALBUTTON_HIGHT           47
#define CALBUTTON_SpaceX          (8+CALBUTTON_WHITH) 
#define CALBUTTON_SpaceY          (10+CALBUTTON_HIGHT)
#define CALBUTTON_LEFT            18
#define CALBUTTON_TOP             (280+90 )
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;
} CALBUTTON;
static const CALBUTTON ButtonData[] = 
{
	/* 1 */
	{ CALBUTTON_LEFT,                    CALBUTTON_TOP,                    CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "BIN" },  //0
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX,   CALBUTTON_TOP,                    CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "OCT" },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*2, CALBUTTON_TOP,                    CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "DEC" },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*3, CALBUTTON_TOP,                    CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "HEX" },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*4, CALBUTTON_TOP,                    CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "<--" }, 
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*5, CALBUTTON_TOP,                    CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "CE"  },


	/* 2 */
	{ CALBUTTON_LEFT,                    CALBUTTON_TOP+CALBUTTON_SpaceY*1, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "MC"  },  //6
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX, CALBUTTON_TOP+CALBUTTON_SpaceY*1, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "MR"  },  
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*2, CALBUTTON_TOP+CALBUTTON_SpaceY*1, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "MS"  },  
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*3, CALBUTTON_TOP+CALBUTTON_SpaceY*1, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "M+"  },  
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*4, CALBUTTON_TOP+CALBUTTON_SpaceY*1, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "M-"   },  
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*5, CALBUTTON_TOP+CALBUTTON_SpaceY*1,  CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "C"   },
	
	
	/* 3 */
	{ CALBUTTON_LEFT,   									CALBUTTON_TOP+CALBUTTON_SpaceY*2, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "A"   },//12
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX, 	CALBUTTON_TOP+CALBUTTON_SpaceY*2, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "B"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*2, CALBUTTON_TOP+CALBUTTON_SpaceY*2, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "C"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*3, CALBUTTON_TOP+CALBUTTON_SpaceY*2, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "D"   }, 
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*4, CALBUTTON_TOP+CALBUTTON_SpaceY*2, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "E"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*5, CALBUTTON_TOP+CALBUTTON_SpaceY*2, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "F"   },
  
	/* 4 */
	{ CALBUTTON_LEFT,   									CALBUTTON_TOP+CALBUTTON_SpaceY*3, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "Mod" },//18
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX, 	CALBUTTON_TOP+CALBUTTON_SpaceY*3, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "+/-" },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*2, CALBUTTON_TOP+CALBUTTON_SpaceY*3, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "7"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*3, CALBUTTON_TOP+CALBUTTON_SpaceY*3, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "8"   }, 
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*4, CALBUTTON_TOP+CALBUTTON_SpaceY*3, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "9"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*5, CALBUTTON_TOP+CALBUTTON_SpaceY*3, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "/"   },

  /* 5 */
	{ CALBUTTON_LEFT,   									CALBUTTON_TOP+CALBUTTON_SpaceY*4, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "RoL" },//24
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX, CALBUTTON_TOP+CALBUTTON_SpaceY*4, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "RoR" },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*2, CALBUTTON_TOP+CALBUTTON_SpaceY*4, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "4"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*3, CALBUTTON_TOP+CALBUTTON_SpaceY*4, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "5"   }, 
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*4, CALBUTTON_TOP+CALBUTTON_SpaceY*4, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "6"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*5, CALBUTTON_TOP+CALBUTTON_SpaceY*4, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "*"   },
  
  /* 6 */
	{ CALBUTTON_LEFT,   										CALBUTTON_TOP+CALBUTTON_SpaceY*5, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "Or"  },//30
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX, 		CALBUTTON_TOP+CALBUTTON_SpaceY*5, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "Xor" },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*2, CALBUTTON_TOP+CALBUTTON_SpaceY*5, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "1"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*3, CALBUTTON_TOP+CALBUTTON_SpaceY*5, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "2"   }, 
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*4, CALBUTTON_TOP+CALBUTTON_SpaceY*5, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "3"   },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*5, CALBUTTON_TOP+CALBUTTON_SpaceY*5, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "-"   },
  
  /* 7 */
	{ CALBUTTON_LEFT,   									CALBUTTON_TOP+CALBUTTON_SpaceY*6, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "Not"  },//36
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX, CALBUTTON_TOP+CALBUTTON_SpaceY*6, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "And"  },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*2, CALBUTTON_TOP+CALBUTTON_SpaceY*6, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "0"    },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*3, CALBUTTON_TOP+CALBUTTON_SpaceY*6, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "+"    }, 
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*4, CALBUTTON_TOP+CALBUTTON_SpaceY*6, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "="    },
	{ CALBUTTON_LEFT+CALBUTTON_SpaceX*5, CALBUTTON_TOP+CALBUTTON_SpaceY*6, CALBUTTON_WHITH,   CALBUTTON_HIGHT,   "Clear"},
};

typedef struct {
  uint8_t IsMemUse;       //【1：使用存储】【0：不使用】
  int32_t MemData;        //存储空间值
  uint8_t DataType;       //【2：二进制】【8：八进制】【10：十进制】【16：十六进制】
  int32_t CalData1;       //存储计算参数
  int32_t CalData2;
  int32_t InputData;      //当前输入值
  int8_t  NegativeFlag;   //【1：当前输入值为正数】【-1：输入值为负数】
  int8_t  Operator1;      //【0:无】【1：+】【2：-】
  int8_t  Operator2;      //【0:无】【1：*】【2：/】【3：Mod】
                          //【4：Or】【5：Xor】【6：And】
  int8_t  NewInputFlag;   //【0：继续输入到原来数据】【1：新的输入数据】
} CALCULATOR_TYPEDEF;
static CALCULATOR_TYPEDEF calculator;
static char tempstr[50];
static TEXT_Handle hTextInput;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateCalculator[] = {
  { FRAMEWIN_CreateIndirect,  "Calculator",            0,      0, 0, 480, 854, 0, 0x0, 0 },
  { TEXT_CreateIndirect,      "Input",      GUI_ID_TEXT0,      10, 280+10, 480-30, 60, 0, 0x20, 0 },
  { TEXT_CreateIndirect,      "Memory",     GUI_ID_TEXT1,      100, 280+12, 20, 20, 0, 0x02, 0 },
  { TEXT_CreateIndirect,      "Datatype",   GUI_ID_TEXT2,      200, 280+12, 80, 20, 0, 0x04, 0 },
  { MULTIEDIT_CreateIndirect, "History",    GUI_ID_MULTIEDIT0, 10, 10, 480-30, 240, 0, 0x0, 0 },
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
/*
* Conver int to string based on radix (usually 2, 8, 10, and 16)
*/
char* cal_itoa(int num,char *str,int radix)
{
  char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  unsigned unum;
  int i=0,j,k;

  if(radix==10&&num<0)
  {
    unum=(unsigned)-num;
    str[i++]='-';
  }
  else unum=(unsigned)num;

  do{
    str[i++]=index[unum%(unsigned)radix];
    unum/=radix;
  }while(unum);
  str[i]='\0';

  if(str[0]=='-')
    k=1;
  else 
    k=0;

  for(j=k;j<=(i-1)/2;j++)
  {
    char temp=str[j];
    str[j]=str[i-1+k-j];
    str[i-1+k-j]=temp;
  }
  return str;
}

int cal_strtol(const char *str,int8_t radix)
{
  char *stop;
  int data;
  data=strtol(str,&stop,radix);
  return data;
}

static void ContrlDispalyWindow(WM_HWIN hWin,uint8_t datatype)
{
  uint8_t i;
  WM_EnableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+33)); //2
  WM_EnableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+34)); //3
  for(i=26;i<29;++i)
  {
    WM_EnableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+i));  //456
  }
  WM_EnableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+20));  //7
  WM_EnableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+21));  //8
  WM_EnableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+22));  //9 
  for(i=12;i<18;++i)
  {
    WM_EnableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+i));  //// ABCDEF
  }
  if(datatype==16)return;
  
  for(i=12;i<18;++i)
  {
    WM_DisableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+i)); // ABCDEF
  }
  if(datatype==10)return;
  
  WM_DisableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+21)); // 8
  WM_DisableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+22)); // 9
  if(datatype==8)return;
  
  WM_DisableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+33)); // 2
  WM_DisableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+34)); // 3
  for(i=26;i<29;++i)
  {
    WM_DisableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+i));  //456
  }
  WM_DisableWindow(WM_GetDialogItem(hWin, ID_CALBUTTON+20));//7
}

static int Calculate_Operator2(void)
{
  int cal_res=0;
  int para1,para2;
  
  if(calculator.Operator2==0)return 0;
  para1=calculator.CalData2;
  para2=calculator.InputData;
  
  switch(calculator.Operator2)
  {
    case 1:  // *
      cal_res=para1*para2;
      break;
    case 2:  // /
      cal_res=para1/para2;
      break;
    case 3:  // Mod
      cal_res=para1%para2;
      break;
    case 4:  //Or
      cal_res=para1|para2;
      break;
    case 5:  //Xor
      cal_res=para1^para2;
      break;
    case 6:  //And
      cal_res=para1&para2;
      break;
    default:
      cal_res=0;
      break;
  }
  calculator.CalData2=0;
  calculator.Operator2=0;
  return cal_res;
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialogCalculator(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  uint8_t i;
  // USER END

  switch (pMsg->MsgId) {
  case WM_DELETE:
		APP_TRACE_DBG(("CalculatorDLG delete\n"));
//		Flag_ICON12 = 0;
		UserApp_Running = 0;
		tpad_flag=0;    
	  break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Calculator'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_CALCULATORDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'Input'
    //
    hTextInput = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetFont(hTextInput, GUI_FONT_32B_1);
    TEXT_SetTextAlign(hTextInput, GUI_TA_RIGHT | GUI_TA_BOTTOM);
    TEXT_SetText(hTextInput,"0");
    TEXT_SetBkColor(hTextInput,GUI_WHITE);
    TEXT_SetTextColor(hTextInput,GUI_BLACK);
    //
    // Initialization of 'Memory'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
    TEXT_SetText(hItem, "M");
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_TOP);
    TEXT_SetTextColor(hItem, GUI_DARKGRAY);
    TEXT_SetBkColor(hItem, GUI_WHITE);
    WM_HideWindow(hItem);
    //
    // Initialization of 'Datatype'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
    TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
    TEXT_SetText(hItem, "DEC");
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_TOP);
    TEXT_SetTextColor(hItem, GUI_DARKGRAY);
    TEXT_SetBkColor(hItem, GUI_WHITE);
    //
    // Initialization of 'History'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0);
    MULTIEDIT_SetText(hItem, "");
    MULTIEDIT_SetTextColor(hItem,MULTIEDIT_CI_READONLY,GUI_BLACK);
    MULTIEDIT_SetFont(hItem, GUI_FONT_20_ASCII);
		MULTIEDIT_SetAutoScrollV(hItem,1);
		MULTIEDIT_SetAutoScrollH(hItem,1); 
    MULTIEDIT_SetMaxNumChars(hItem,2048);
    MULTIEDIT_SetReadOnly(hItem,1);
    // USER START (Optionally insert additional code for further widget initialization)
    for (i = 0; i < GUI_COUNTOF(ButtonData); i++) 
    {
      hItem = BUTTON_CreateEx(ButtonData[i].xPos, ButtonData[i].yPos, ButtonData[i].xSize, ButtonData[i].ySize, 
      WM_GetClientWindow(pMsg->hWin), WM_CF_SHOW, 0, ID_CALBUTTON + i);
      BUTTON_SetFont(hItem, GUI_FONT_24B_ASCII);
      BUTTON_SetText(hItem, ButtonData[i].acLabel);
      BUTTON_SetTextAlign(hItem,GUI_TA_HCENTER|GUI_TA_VCENTER);
    }  
    ContrlDispalyWindow(pMsg->hWin,calculator.DataType);    
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
					// USER END
				break;
			case WM_NOTIFICATION_RELEASED:
					LED1_OFF;
          switch(Id)
          {            
            case  ID_CALBUTTON+0:  //BIN
              {
                calculator.DataType=2;
                calculator.NewInputFlag=1; 
                TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2),"BIN");
                cal_itoa(calculator.InputData,tempstr,2);
                TEXT_SetText(hTextInput,tempstr);
                ContrlDispalyWindow(pMsg->hWin,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"BIN 0b");
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n");
              }
              break;
            case  ID_CALBUTTON+1:  //OCT
              {
                calculator.DataType=8;
                calculator.NewInputFlag=1; 
                TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2),"OCT");
                sprintf(tempstr,"%o",calculator.InputData);
                TEXT_SetText(hTextInput,tempstr);
                ContrlDispalyWindow(pMsg->hWin,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"OCT 0");
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n");
              }
              break;
            case  ID_CALBUTTON+2:  //DEC
              {
                calculator.DataType=10;
                calculator.NewInputFlag=1; 
                TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2),"DEC");
                sprintf(tempstr,"%d",calculator.InputData);
                TEXT_SetText(hTextInput,tempstr);
                ContrlDispalyWindow(pMsg->hWin,calculator.DataType);   
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"DEC ");
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
              }
              break;
            case  ID_CALBUTTON+3: //HEX
              {
                calculator.DataType=16;
                calculator.NewInputFlag=1; 
                TEXT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2),"HEX");
                sprintf(tempstr,"%X",calculator.InputData);
                TEXT_SetText(hTextInput,tempstr);
                ContrlDispalyWindow(pMsg->hWin,calculator.DataType); 
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"HEX 0x");
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n");
              }
              break;
            case  ID_CALBUTTON+4: //<-- backspace
              {
                if(calculator.NewInputFlag==1)break;
                TEXT_GetText(hTextInput,tempstr,50);
                if(strlen(tempstr)==0)
                {
                  calculator.NegativeFlag=1;
                  calculator.NewInputFlag=1;
                  calculator.InputData=0;
                  break;
                }
                tempstr[strlen(tempstr)-1]='\0';
                TEXT_SetText(hTextInput,tempstr);
                calculator.InputData=cal_strtol(tempstr,calculator.DataType);
              }
              break;
            case  ID_CALBUTTON+5:  //CE Clear Error
              {
                calculator.NegativeFlag=1;
                calculator.NewInputFlag=1;
                TEXT_SetText(hTextInput,"0");
                calculator.InputData=0;      
              }
              break;
            case  ID_CALBUTTON+11: //C Clear              
              {     
                calculator.Operator1=0;
                calculator.Operator2=0;
                calculator.NewInputFlag=1;
                calculator.NegativeFlag=1;
                TEXT_SetText(hTextInput,"0");
                calculator.InputData=0;                
                calculator.CalData1=0;
                calculator.CalData2=0;
              }
              break;
            case  ID_CALBUTTON+6: //MC MemoryClear
              {
                WM_HideWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1));
                calculator.MemData=0;
                calculator.IsMemUse=0;        
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," MC \n");
              }
              break;
            case  ID_CALBUTTON+7: //MR MemoryRead
              {
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                }
                if(calculator.IsMemUse!=0)
                {
                  calculator.InputData=calculator.MemData;
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);   
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"\n MR \n");                    
                } 
              }
              break;
            case  ID_CALBUTTON+8: //MS MemorySet
              {
                calculator.IsMemUse=1;
                calculator.MemData=calculator.InputData;
                WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1));
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," MS \n");
              }
              break;
            case  ID_CALBUTTON+9: //M+ MemoryPlus
              {
                calculator.MemData += calculator.InputData;
                WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1)); 
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," M+ \n");
              }
              break;
            case  ID_CALBUTTON+10: //M- MemoryMinus
              {
                calculator.MemData -= calculator.InputData;
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," M- \n");
              }
              break;
            case  ID_CALBUTTON+41: //Clear history   
              {
                MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0), "");
              }
              break;
            case  ID_CALBUTTON+12:   //A
            case  ID_CALBUTTON+13:   //B
            case  ID_CALBUTTON+14:  //C
            case  ID_CALBUTTON+15:  //D
            case  ID_CALBUTTON+16:  //E
            case  ID_CALBUTTON+17:  //F
              {
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                }
                if(calculator.InputData&0xf0000000) break;
                calculator.InputData=calculator.InputData*16+calculator.NegativeFlag*(Id-(ID_CALBUTTON+12)+10);
                sprintf(tempstr,"%X",calculator.InputData);
                TEXT_SetText(hTextInput,tempstr); 
              }
              break;
            case  ID_CALBUTTON+20:  //7    
              {  
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                }                
                if(calculator.DataType==10)
                {
                  if(calculator.InputData>=214748365)break;
                  if(calculator.InputData<=-214748365)break;  
                }                                
                if((calculator.DataType==8)&&(calculator.InputData&0xe0000000))break;
                if((calculator.DataType==16)&&(calculator.InputData&0xf0000000))break;
                calculator.InputData=calculator.InputData*calculator.DataType+calculator.NegativeFlag*(Id-(ID_CALBUTTON+20)+7);
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
              }
              break;
            case  ID_CALBUTTON+21:  //8
            case  ID_CALBUTTON+22:  //9
              {
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                } 
                if(calculator.DataType==10)
                {
                  if(calculator.InputData>=214748364)break;
                  if(calculator.InputData<=-214748365)break;
                  if((Id==ID_CALBUTTON+19)&&(calculator.InputData<=-214748364))break;                  
                }
                if((calculator.DataType==16)&&(calculator.InputData&0xf0000000))break;
                if((calculator.DataType==8)&&(calculator.InputData&0xe0000000))break;
                calculator.InputData=calculator.InputData*calculator.DataType+calculator.NegativeFlag*(Id-(ID_CALBUTTON+21)+8);
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
              }
              break;
            case  ID_CALBUTTON+26:  //4
            case  ID_CALBUTTON+27:  //5
            case  ID_CALBUTTON+28:  //6
              {
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                }                 
                if(calculator.DataType==10)
                {
                  if(calculator.InputData>=214748365)break;
                  if(calculator.InputData<=-214748365)break;  
                }                
                if((calculator.DataType==16)&&(calculator.InputData&0xf0000000))break;
                if((calculator.DataType==8)&&(calculator.InputData&0xe0000000))break;
                calculator.InputData=calculator.InputData*calculator.DataType+calculator.NegativeFlag*(Id-(ID_CALBUTTON+26)+4);
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
              }
              break;
            case  ID_CALBUTTON+32:  //1
              {
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                }                 
                if(calculator.DataType==10)
                {
                  if(calculator.InputData>=214748365)break;
                  if(calculator.InputData<=-214748365)break;  
                }
                if((calculator.DataType==16)&&(calculator.InputData&0xf0000000))break;                
                if((calculator.DataType==2)&&(calculator.InputData&0x80000000))break;
                if((calculator.DataType==8)&&(calculator.InputData&0xe0000000))break;
                calculator.InputData=calculator.InputData*calculator.DataType+calculator.NegativeFlag;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
              }              
              break;
            case  ID_CALBUTTON+33:  //2
            case  ID_CALBUTTON+34:  //3
              {
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                }                 
                if(calculator.DataType==10)
                {
                  if(calculator.InputData>=214748365)break;
                  if(calculator.InputData<=-214748365)break;  
                }
                if((calculator.DataType==16)&&(calculator.InputData&0xf0000000))break;
                if((calculator.DataType==8)&&(calculator.InputData&0xe0000000))break;
                calculator.InputData=calculator.InputData*calculator.DataType+calculator.NegativeFlag*(Id-(ID_CALBUTTON+33)+2);
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
              } 
              break;
            case  ID_CALBUTTON+38:  //0
              {
                if(calculator.NewInputFlag==1)
                {
                  calculator.NewInputFlag=0;
                  calculator.InputData=0;
                }                 
                if(calculator.DataType==10)
                {
                  if(calculator.InputData>=214748365)break;
                  if(calculator.InputData<=-214748365)break;  
                }
                if((calculator.DataType==16)&&(calculator.InputData&0xf0000000))break;
                if((calculator.DataType==2)&&(calculator.InputData&0x80000000))break;
                if((calculator.DataType==8)&&(calculator.InputData&0xe0000000))break;
                calculator.InputData = calculator.InputData*calculator.DataType;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
              } 
              break;
            case  ID_CALBUTTON+19: //+/-
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," +/- =");
                if(calculator.NegativeFlag==1)
                  calculator.NegativeFlag=-1;
                else
                  calculator.NegativeFlag=1;
                calculator.InputData=-1*calculator.InputData;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
                if(calculator.DataType!=10)calculator.NewInputFlag=1;                  
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n");             
              } 
              break;
            case  ID_CALBUTTON+18:  //Mod
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," Mod ");
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {                  
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }                
                calculator.Operator2=3;
                calculator.CalData2=calculator.InputData;
                calculator.NewInputFlag=1;  
                calculator.NegativeFlag=1;                
              } 
              break;
            case  ID_CALBUTTON+24:  //RoL
              {
                uint32_t highestbit;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," RoL =");
                highestbit=(calculator.InputData&(uint32_t)0x80000000)?0x00000001:0x00000000;
                calculator.InputData=((calculator.InputData<<1)&0xfffffffe)|highestbit;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
                calculator.NewInputFlag=1;
                calculator.NegativeFlag=1;
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n");
              } 
              break;
            case  ID_CALBUTTON+25:  //RoR
              {
                uint32_t lowestbit;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," RoR =");
                lowestbit=(calculator.InputData&(uint32_t)0x00000001)?0x80000000:0x00000000;
                calculator.InputData=((calculator.InputData>>1)&0x7fffffff)|lowestbit;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
                calculator.NewInputFlag=1;
                calculator.NegativeFlag=1;
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n");
              } 
              break;
            case  ID_CALBUTTON+30:  //Or
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," Or ");
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }
                calculator.Operator2=4;
                calculator.CalData2=calculator.InputData;
                calculator.NewInputFlag=1; 
                calculator.NegativeFlag=1;
              } 
              break;
            case  ID_CALBUTTON+31:  //Xor
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," Xor ");
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }
                calculator.Operator2=5;
                calculator.CalData2=calculator.InputData;
                calculator.NewInputFlag=1; 
                calculator.NegativeFlag=1;
              } 
              break;
            case  ID_CALBUTTON+36:  //Not
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," Not \n=");
                calculator.InputData=~calculator.InputData;
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                TEXT_SetText(hTextInput,tempstr);
                calculator.NewInputFlag=1;
                calculator.NegativeFlag=1;
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n");                 
              } 
              break;
            case  ID_CALBUTTON+37:  //And
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," And ");
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }
                calculator.Operator2=6;
                calculator.CalData2=calculator.InputData;
                calculator.NewInputFlag=1; 
                calculator.NegativeFlag=1;
              } 
              break;
            case  ID_CALBUTTON+23:  // /
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," / ");
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }
                calculator.Operator2=2;
                calculator.CalData2=calculator.InputData;
                calculator.NewInputFlag=1; 
                calculator.NegativeFlag=1;
              } 
              break;
            case  ID_CALBUTTON+29:  // *
              {
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," * ");
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {                  
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }
                calculator.Operator2=1;
                calculator.CalData2=calculator.InputData;
                calculator.NewInputFlag=1; 
                calculator.NegativeFlag=1;
              } 
              break;
            case  ID_CALBUTTON+35:  // -
              {    
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);                
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {                  
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                } 
                if(calculator.Operator1 && (calculator.NewInputFlag==0))
                {                  
                  if(calculator.Operator1==1)
                    calculator.InputData=calculator.CalData1+calculator.InputData;
                  else
                    calculator.InputData=calculator.CalData1-calculator.InputData;
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                } 
                
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," - ");               
                calculator.Operator1=2;
                calculator.CalData1=calculator.InputData;                
                calculator.NewInputFlag=1;
                calculator.NegativeFlag=1;
              } 
              break;
            case  ID_CALBUTTON+39:  // +
              {      
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);           
                if(calculator.Operator2 && (calculator.NewInputFlag==0))
                {                  
                  calculator.InputData=Calculate_Operator2();
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }
                if(calculator.Operator1 && (calculator.NewInputFlag==0))
                {                  
                  if(calculator.Operator1==1)
                    calculator.InputData=calculator.CalData1+calculator.InputData;
                  else
                    calculator.InputData=calculator.CalData1-calculator.InputData;
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),"=");
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }                
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," + ");                
                calculator.Operator1=1;
                calculator.CalData1=calculator.InputData;                
                calculator.NewInputFlag=1;
                calculator.NegativeFlag=1;
              } 
              break;
            case  ID_CALBUTTON+40:  // =
              {                
                cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," = ");
                if(calculator.NewInputFlag==0)
                {
                  if(calculator.Operator1)
                  {
                    if(calculator.Operator1==1)
                      calculator.InputData=calculator.CalData1+calculator.InputData;
                    else
                      calculator.InputData=calculator.CalData1-calculator.InputData;
                  }
                  else if(calculator.Operator2)
                  {
                    calculator.InputData=Calculate_Operator2();                  
                  }
                  cal_itoa(calculator.InputData,tempstr,calculator.DataType);
                  TEXT_SetText(hTextInput,tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0),tempstr);
                  MULTIEDIT_AddText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0)," \n"); 
                }                
                calculator.Operator1=0;
                calculator.Operator2=0;
                calculator.NewInputFlag=1;
                calculator.NegativeFlag=1;
                calculator.CalData1=0;
                calculator.CalData2=0;
              } 
              break;
            default:
              break;
          }
        break;
    }
   break;
  // USER START (Optionally insert additional message handling)
  case WM_PAINT:
//    GUI_SetBkColor(APPBKCOLOR);
//    GUI_Clear();
    GUI_SetColor(GUI_CYAN);
    GUI_FillRect(10,365,480-20,769);
	
    GUI_SetColor(GUI_GREEN);
    GUI_FillRect(15,536,480-25,764);
	
    GUI_SetColor(GUI_BLUE);
    GUI_FillRect(15,479,480-25,536);
	
    GUI_FillRect(159,530,378,707);
									
    GUI_FillRect(159,655,232,764);
									
    WM_SetFocus(pMsg->hWin);
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
*       CreateCalculator
*/
void FUN_ICON12Clicked(void)  
{
  WM_HWIN hWin;
  APP_TRACE_DBG(("CalculatorDLG create\n"));
  calculator.InputData=0;
  calculator.DataType=10;
  calculator.NegativeFlag=1;
  calculator.IsMemUse=0;
  calculator.NewInputFlag=1;
  calculator.Operator1=0;
  calculator.Operator2=0;
  hWin = GUI_CreateDialogBox(_aDialogCreateCalculator, GUI_COUNTOF(_aDialogCreateCalculator), _cbDialogCalculator, WM_HBKWIN, 0, 0);
  while(UserApp_Running)
  {
    if(tpad_flag)WM_DeleteWindow(hWin);  
    GUI_Delay(10);
  }
}

// USER START (Optionally insert additional public code)
// USER END
/*************************** End of file ****************************/
