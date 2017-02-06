/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emwin触摸测试程序
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

#include "GUIDEMO.h"


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

	printf("\r\n ********** emwin触摸测试程序*********** \r\n"); 
	
	//emwin官方demo，修改guidemo.h文件中的宏可增加演示应用
	GUIDEMO_Main();

}





/*********************************************END OF FILE**********************/

