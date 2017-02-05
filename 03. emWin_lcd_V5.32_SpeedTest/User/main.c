/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emwin速度测试程序
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

extern void SpeedTest(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{
  LED_GPIO_Config();
	LED_GREEN;
	
  /* 初始化定时器 */
	SysTick_Init();
	
	/*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);  
	
	/* 初始化GUI */
	GUI_Init();

	Debug_USART_Config();		
	
	printf("\r\n ********** emwin速度测试程序*********** \r\n"); 
	
 GUI_SetColor(GUI_WHITE);   
	/* 显示测试 */
	GUI_DispStringAt(" emWin Speed test Demo",10,10);
	GUI_Delay(2000);
   

	//移植本程序时，注意要把启动文件中的栈空间设置大一点

	while(1)
	{
		//执行速度测试Demo
		SpeedTest();
		GUI_Delay(3000);
	}
}





/*********************************************END OF FILE**********************/

