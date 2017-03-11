/**
  ******************************************************************************
  * @file    WeatherDLG.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   天气的应用窗口
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
#define UTF8_WETHERDLG      "\xe6\xb8\xa9\xe6\xb9\xbf\xe5\xba\xa6&\xe5\x85\x89\xe7\x85\xa7"//温湿度&光照
#define UTF8_TEMPERATURE    "\xe6\xb8\xa9\xe5\xba\xa6"//温度
#define UTF8_HUMIDITY       "\xe6\xb9\xbf\xe5\xba\xa6"//湿度
#define UTF8_ILLUMINATION   "\xe5\x85\x89\xe7\x85\xa7"//光照
#define UTF8_PHOTORESISTOR  "\xe5\x85\x89\xe6\x95\x8f\xe7\x94\xb5\xe9\x98\xbb"//光敏电阻
#define UTF8_ADSAMPLEVALUE  "AD\xe9\x87\x87\xe6\xa0\xb7\xe5\x80\xbc\xef\xbc\x9a"//AD采样值
#define UTF8_VOLAGEVALUE    "\xe7\x94\xb5\xe5\x8e\x8b\xe5\x80\xbc\xef\xbc\x9a"//电压值
#define UTF8_TYPE           "\xe6\x9b\xb2\xe7\xba\xbf\xe5\x9b\xbe\xe7\xb1\xbb\xe5\x9e\x8b\xef\xbc\x9a"//曲线图类型：
#define UTF8_CENTIGRADE     "\xe2\x84\x83"//℃
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
  uint8_t Sensor;              /* 1:检测到DS18B20  2:检测到DHT11  0:无1，2 */
  uint8_t WaveTpye;            /* 波形显示内容：0：温度，1：湿度，2：照度 */
  float Temperature;
  float Humidity;
  uint16_t ADC_Value;
}DRV_WEATHER;

static DHT11_Data_TypeDef DHT11_Data;
static DRV_WEATHER  drv_weather={0,2,0.0,0.0,0};   
static GRAPH_DATA_Handle  	hData_Weather;

// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreateWeather[] = {
  { FRAMEWIN_CreateIndirect, "Weather",      0,              0,   0,   480, 854, 0, 0x0,    0 },
  { TEXT_CreateIndirect,     "Temperature",  GUI_ID_TEXT0,   100,  80,  75,  35,  0, 0x64,   0 },
  { TEXT_CreateIndirect,     "Humidity",     GUI_ID_TEXT1,   480/2+80,  80, 75,  35,  0, 0x64,   0 }, 
  { TEXT_CreateIndirect,     "SampleValue",  GUI_ID_TEXT2,   130, 225, 100, 35,  0, 0x64,   0 }, 
  { TEXT_CreateIndirect,     "VoltageValue", GUI_ID_TEXT3,   480/2+95,  225, 75,  35,  0, 0x64,   0 },	
  { RADIO_CreateIndirect,    "DataType",     GUI_ID_RADIO0,  190, 320,  100,  120,  0, 0x2403, 0 },
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
static void _cbDialogWeather(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  char tempstr[30];
  // USER END

  switch (pMsg->MsgId) {
  case WM_DELETE:
		// USER START (Optionally insert additional code for further widget initialization)
		APP_TRACE_DBG(("WeatherDLG delete\n"));	   
//    TEMP_HUM_GPIO_Config();  
	  Photoresister_TurnOFF();
//		Flag_ICON3 = 0;
		UserApp_Running = 0;
    tpad_flag=0;    
		// USER END
	  break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Weather'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetText(hItem,UTF8_WETHERDLG);
    FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
    //
    // Initialization of 'Temperature'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetText(hItem,"0.00");
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    //
    // Initialization of 'DataType'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0);
    RADIO_SetFont(hItem, &FONT_XINSONGTI_25);
    RADIO_SetText(hItem, UTF8_TEMPERATURE, 0);
    RADIO_SetText(hItem, UTF8_HUMIDITY, 1);
    RADIO_SetText(hItem, UTF8_ILLUMINATION, 2);  
    RADIO_SetValue(hItem,drv_weather.WaveTpye);    
    //
    // Initialization of 'Humidity'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT1);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetText(hItem, "0.00");
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'SampleValue'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT2);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetText(hItem, "0");
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER); 
		//
    // Initialization of 'VoltageValue'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT3);
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    TEXT_SetText(hItem, "0");
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER); 
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_RADIO0: // Notifications sent by 'DataType'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        GRAPH_DATA_YT_Clear(hData_Weather);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        drv_weather.WaveTpye=RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0));        
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
		//边框
	  GUI_SetColor(GUI_BLUE);  
    GUI_DrawRect(10,20,480-20,130);
    GUI_DrawRect(10,170,480-20,270);
	  GUI_DrawRect(10,290,480-20,760);
    GUI_SetColor(GUI_LIGHTBLUE);
	  //边框说明文字
    sprintf(tempstr,"%s&%s",UTF8_TEMPERATURE,UTF8_HUMIDITY);
    GUI_DispStringAt(tempstr,20,10);
    GUI_DispStringAt(UTF8_ILLUMINATION,20,160);
  
	  //温度
    GUI_SetColor(GUI_LIGHTGREEN);  
    GUI_SetFont(&FONT_XINSONGTI_25);
    sprintf(tempstr,"%s:",UTF8_TEMPERATURE);
    GUI_DispStringAt(tempstr,30,85);
	  GUI_DispStringAt(UTF8_CENTIGRADE,178,85);

	  //湿度
    sprintf(tempstr,"%s:",UTF8_HUMIDITY);
    GUI_DispStringAt(tempstr,480/2+30,85);
	  GUI_DispStringAt("%",400,85);

	  //光敏电阻
		GUI_SetColor(GUI_ORANGE);
    GUI_DispStringHCenterAt(UTF8_PHOTORESISTOR,480/2,190); 
		
		GUI_SetColor(GUI_LIGHTGREEN);  
    GUI_DispStringAt(UTF8_ADSAMPLEVALUE,20,230);	
	  GUI_DispStringAt(UTF8_VOLAGEVALUE,480/2+20,230);
    GUI_DispStringAt("mV",415,230);  
	
	  //曲线图类型
    GUI_DispStringAt(UTF8_TYPE,30,340);
  
    GUI_SetFont(&FONT_XINSONGTI_25);
    
    GUI_SetColor(GUI_ORANGE);
    if(drv_weather.Sensor==1)
    {
      GUI_DispStringHCenterAt("(DS18B20)",480/2,40);
    }
    else if(drv_weather.Sensor==2)
    {
      GUI_DispStringHCenterAt("(DHT11)",480/2,40);
    }
    else
    {
      GUI_DispStringHCenterAt("(NO SENSOR)",480/2,40);
    }
      
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
  * @brief  点击图标后的动作函数，DHT11/DS18B20/光敏电阻
  * @param  无
  * @retval 无
  */
void FUN_ICON3Clicked(void)  
{
  OS_ERR      err;
  WM_HWIN hWin;
  uint8_t i;
  uint8_t errorflag=0;
  char tempstr[10];
  WM_HWIN         	  hGraph;
  GRAPH_SCALE_Handle 	hScaleVert,hScaleHori;
  
  APP_TRACE_DBG(("WeatherDLG create\n"));
  
  OSSchedLock(&err);
  if(DS18B20_Init()==0) 	
	{
		drv_weather.Sensor=1;					        /*----  DS18B20 -----*/
		drv_weather.Temperature=DS18B20_Get_Temp();	
    drv_weather.Humidity=0.0;
	}
	else 
  {
    DHT11_GPIO_Config();
    if(Read_DHT11(&DHT11_Data)==1)			    
	  {
		  drv_weather.Sensor=2;								/*----  DHT11 -----*/
      drv_weather.Humidity=0.0;
      drv_weather.Temperature=0.0;
	  }
    else
      drv_weather.Sensor=0;
  }
  
  Photoresister_Init();                        /*----  光敏电阻 -----*/
  
  OSSchedUnlock(&err);
  
	//创建界面
  hWin = GUI_CreateDialogBox(_aDialogCreateWeather, GUI_COUNTOF(_aDialogCreateWeather), _cbDialogWeather, WM_HBKWIN, 0, 0);
  //曲线图
	hGraph      = GRAPH_CreateEx (15, 430, 440, 300, WM_GetClientWindow(hWin), WM_CF_SHOW| WM_CF_CONST_OUTLINE , 0, GUI_ID_GRAPH0);
  WM_SetHasTrans (hGraph);
  GRAPH_SetGridVis(hGraph, 1);
  GRAPH_SetBorder(hGraph,0,0,0,0);
  GRAPH_SetGridDistY(hGraph, 50);
  GRAPH_SetGridDistX(hGraph, 50);
  WM_BringToBottom  (hGraph);
  GRAPH_SetGridFixedX(hGraph, 1);	
  WIDGET_SetEffect(hGraph, &WIDGET_Effect_Simple);
  
  hData_Weather = GRAPH_DATA_YT_Create(GUI_RED,250,0,0);
	GRAPH_DATA_YT_SetAlign(hData_Weather,  GRAPH_ALIGN_RIGHT);	
  GRAPH_AttachData (hGraph, hData_Weather);
  
	hScaleVert  = GRAPH_SCALE_Create(4,GUI_TA_VCENTER,GRAPH_SCALE_CF_VERTICAL,50);//必须使能FPU功能，否则会进入hardfault

	hScaleHori = GRAPH_SCALE_Create(280, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
    
  GRAPH_SCALE_SetFactor   (hScaleVert, 1);
	GRAPH_SCALE_SetFactor   (hScaleHori, 1);
  
	GRAPH_SCALE_SetTextColor(hScaleHori, GUI_YELLOW);
	GRAPH_SCALE_SetTextColor(hScaleVert, GUI_YELLOW);
  GRAPH_SCALE_SetFont(hScaleHori,GUI_FONT_16_ASCII);
  GRAPH_SCALE_SetFont(hScaleVert,GUI_FONT_16_ASCII);
  
	GRAPH_AttachScale(hGraph, hScaleHori);
	GRAPH_AttachScale(hGraph, hScaleVert);
  
  
  while(UserApp_Running)
  {
    i++;
    if(i>=100)
    {      
      OSSchedLock(&err);
			if(drv_weather.Sensor==2)
			{
				if(Read_DHT11(&DHT11_Data)==0)
        {
          errorflag++;
          if(errorflag==5) drv_weather.Sensor=0;
        }
        else
        {
          errorflag=0;
        }
        drv_weather.Humidity=(DHT11_Data.humi_int*100+DHT11_Data.humi_deci)/100;
        drv_weather.Temperature=(DHT11_Data.temp_int*100+DHT11_Data.temp_deci)/100;
        sprintf(tempstr,"%d.%d",DHT11_Data.temp_int,DHT11_Data.temp_deci);
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),tempstr);
        sprintf(tempstr,"%d.%d",DHT11_Data.humi_int,DHT11_Data.humi_deci);
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),tempstr);
        if(drv_weather.WaveTpye==0)
        {
          GRAPH_SCALE_SetFactor(hScaleVert, 0.2f);
          GRAPH_DATA_YT_AddValue(hData_Weather,drv_weather.Temperature*5);
          i=0;
        }
        else if(drv_weather.WaveTpye==1)
        {
          GRAPH_SCALE_SetFactor(hScaleVert, 0.3f);
          GRAPH_DATA_YT_AddValue(hData_Weather,drv_weather.Humidity*333/100);
          i=0;
        }
			}
			else if(drv_weather.Sensor==1)
			{
				drv_weather.Temperature=DS18B20_Get_Temp();
				
				  APP_TRACE_DBG(("\r\nDS18B20=%f",drv_weather.Temperature));

        if(drv_weather.Temperature==0.0625f)
        {
          errorflag++;
          if(errorflag==5) drv_weather.Sensor=0;
          drv_weather.Temperature=0;
        }
        else
        {
          errorflag=0;
        }      
				sprintf(tempstr,"%0.2f",drv_weather.Temperature);
				TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT0),tempstr);
        TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT1),"0.00");
        if(drv_weather.WaveTpye==0)
        {
          GRAPH_SCALE_SetFactor(hScaleVert, 0.3f);
          GRAPH_DATA_YT_AddValue(hData_Weather,drv_weather.Temperature*333/100);
          i=20;
        }
			}
			drv_weather.ADC_Value=Photoresister_ADC_ConvertedValue*3300/4096;
			sprintf(tempstr,"0x%X",Photoresister_ADC_ConvertedValue);			
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT2),tempstr);
			sprintf(tempstr,"%d",drv_weather.ADC_Value);			
			TEXT_SetText(WM_GetDialogItem(hWin, GUI_ID_TEXT3),tempstr);
			if(drv_weather.WaveTpye==2)
			{
				GRAPH_SCALE_SetFactor(hScaleVert, 12);
				GRAPH_DATA_YT_AddValue(hData_Weather,drv_weather.ADC_Value/12);
				i=50;
			}
			OSSchedUnlock(&err);
    }
    
    if(tpad_flag)WM_DeleteWindow(hWin);
    GUI_Delay(10);
  }
}
// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
