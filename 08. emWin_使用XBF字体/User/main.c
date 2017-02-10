/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emWin_lcd_V532_FatFs移植
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f4xx.h"
#include "GUI.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./lcd/bsp_ili9806g_lcd.h"
#include "./systick/bsp_SysTick.h"
#include "./touch/gt5xx.h"
#include "./sram/bsp_sram.h"	  
#include "ff.h"

#include "GUIFont_Port.h"   



extern void Fatfs_MainTask(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{
	 //初始化外部SRAM  
  FSMC_SRAM_Init();

  LED_GPIO_Config();
	LED_GREEN;	
	
	/*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
	
	/* 初始化GUI */
	GUI_Init();	

	//初始化串口
	Debug_USART_Config();	

	//触摸初始化有使用串口输出调试信息，为确保正常，先初始化串口再初始化触摸
	GTP_Init_Panel();
	 
	/* 初始化定时器 */
	SysTick_Init();

	/* 串口调试信息 */
	printf("emWin FatFs demo\r\n");
	
	//创建XBF字体，使用UTF8编码
	Creat_XBF_Font();
	GUI_UC_SetEncodeUTF8();
	GUI_SetDefaultFont(&FONT_XINSONGTI_25);

	
	Fatfs_MainTask();

}





/*********************************************END OF FILE**********************/

