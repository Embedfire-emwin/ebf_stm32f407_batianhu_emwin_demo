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
#include "./key/bsp_key.h" 

#include "ff.h"

#include "GUIFont_Port.h"   

FATFS fs;													/* Work area (file system object) for logical drives */


extern void drawbmp(void);
extern void drawJPEG(void);
extern void drawpng(void);
extern void drawgif(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main ( void )
{
	static uint8_t show_pic_flag = 2;
	
	GUI_RECT info_text ={0,100,480,200};

	 //初始化外部SRAM  
  FSMC_SRAM_Init();

  LED_GPIO_Config();
	Key_GPIO_Config();
	
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
	
	GUI_SetBkColor(GUI_WHITE);
  GUI_Clear(); 
  GUI_SetFont(GUI_FONT_32B_ASCII);
  GUI_SetColor(GUI_BLUE);
	
	info_text.x0 = 10;
	info_text.x1 = 480-10;
	info_text.y0 = 100;
	info_text.y1 = 200;

	GUI_DispStringInRectWrap("Press 'KEY1' to show different format picture",
															&info_text,
															GUI_TA_CENTER,
															GUI_WRAPMODE_WORD) ;
	
	//挂载文件系统，必须先挂载文件系统再显示文件系统中的图片	
	f_mount(&fs,"1:",1);	
	
	while(1)
	{	
		if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
		{
			show_pic_flag++;
			
			switch(show_pic_flag%4)
			{
				case 0:drawpng();			break;
				case 1:drawbmp();			break;
				case 2:drawJPEG();   	break; 
				case 3:drawgif();   	break; 				

			}
			LED2_TOGGLE;
		}   
    
	  GUI_Delay(10);
	}
}





/*********************************************END OF FILE**********************/

