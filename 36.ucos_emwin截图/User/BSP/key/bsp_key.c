/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "./key/bsp_key.h" 
#include "os.h"

OS_FLAG_GRP 	key_flag_grp;                   //按键事件标志组


/// 不精确的延时
void Key_Delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
} 

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
	
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN; 
  
  /*设置引脚为输入模式*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  
  /*设置引脚不上拉也不下拉*/
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
  /*使用上面的结构体初始化按键*/
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);   
  
  /*选择按键的引脚*/
	GPIO_InitStructure.GPIO_Pin = KEY2_PIN; 
  
  /*使用上面的结构体初始化按键*/
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);  
}

/**
  * @brief   检测是否有按键按下     
  *	@param 	GPIOx:具体的端口, x可以是（A...K） 
	*	@param 	GPIO_PIN:具体的端口位， 可以是GPIO_PIN_x（x可以是0...15）
  * @retval  按键的状态
  *		@arg KEY_ON:按键按下
  *		@arg KEY_OFF:按键没按下
  */
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
		OS_ERR  os_err;

	/*检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*等待按键释放 */
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON)
		{
					OSTimeDly(2u, 
									OS_OPT_TIME_DLY,
									&os_err);

		}			
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}

#include "./led/bsp_led.h"   
#include "./usart/bsp_debug_usart.h"

extern uint16_t LCD_X_LENGTH ;
extern uint16_t LCD_Y_LENGTH ;
extern void emWin_CreateBMPPicture(uint8_t * FileName, int x0, int y0, int xSize, int ySize);


/**
  * @brief  按键任务函数
  * @param  无
  * @retval 无
  */
void  BSPTaskKeyScan  (void )
{
	OS_ERR  os_err;
	
	  /* 创建事件标志组 flag_grp */
  OSFlagCreate ((OS_FLAG_GRP  *)&key_flag_grp,        //指向事件标志组的指针
                (CPU_CHAR     *)"KEY Flag",  //事件标志组的名字
                (OS_FLAGS      )0,                //事件标志组的初始值
                (OS_ERR       *)&os_err);					  //返回错误类型


	while(1)
	{
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON  )
			{
						OSFlagPost ((OS_FLAG_GRP  *)&key_flag_grp,                             //将标志组的BIT0清0
													(OS_FLAGS      )EVEN_KEY1_DOWN,
													(OS_OPT        )OS_OPT_POST_FLAG_SET,
													(OS_ERR       *)&os_err);
			}
			else
			{
			
			}
			
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_PIN) == KEY_ON  )
			{
				
				
							static uint8_t name_count = 0;
							char name[100];
								
//				      OSSchedLock(&os_err);

							//用来设置截图名字，防止重复，实际应用中可以使用系统时间来命名。
							name_count++; 
							sprintf(name,"0:screen_shot_%d.bmp",name_count);

							LED_BLUE;
							printf("\r\n正在截图...");
														
							emWin_CreateBMPPicture((uint8_t *)name,0,0,LCD_X_LENGTH,LCD_Y_LENGTH);
				
							printf("\r\n截图完成！");
							
//							OSSchedUnlock(&os_err);
//							OSFlagPost ((OS_FLAG_GRP  *)&key_flag_grp,                             //将标志组的BIT0清0
//														(OS_FLAGS      )EVEN_KEY2_DOWN,
//														(OS_OPT        )OS_OPT_POST_FLAG_SET,
//														(OS_ERR       *)&os_err);
			}
			else
			{
			
			}			

			//延时，所有任务函数的死循环内都应有至少1ms延时
		  //特别是高优先级的任务，若无延时，其它低优先级任务可能会无机会执行
			OSTimeDly(15u, 
									OS_OPT_TIME_DLY,
									&os_err);

			
	}
}
/*********************************************END OF FILE**********************/
