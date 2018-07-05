/**
  ******************************************************************************
  * @file    GUIBaseTest.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   emwin基础显示例程
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include <includes.h>


/**
  * @brief  GUIBase_Test，任务函数，emwin基础显示测试，
  * @param  无  
  * @retval 无
  */
void GUIBase_Test ( void )
{	
	int i=0;
	char str_temp[50];
	OS_ERR      os_err;


	//清除屏幕
	GUI_Clear();

	printf("\r\n **********  emwin基础显示例程*********** \r\n"); 
	printf("\r\n 本程序不支持中文 \r\n"); 

	GUI_SetColor(GUI_WHITE);   
	/* 显示测试 */
	GUI_DispStringAt("wildfire F407 emWin test!",10,10);
	GUI_Delay(100);
	
	while (1)
	{
			//显示数字
			GUI_DispDecAt(i++,50,50,4);
			if(i>9999)
				i=0; 
			
			//显示CPU使用率
			sprintf(str_temp,"cpu usage:%.2f%%",((float)OSStatTaskCPUUsage/100));
			GUI_DispStringAtCEOL(str_temp,10,100);

		
			//延时，所有任务函数的死循环内都应有至少1ms延时
		  //特别是高优先级的任务，若无延时，其它低优先级任务可能会无机会执行
			
			//使用GUI_Delay是等效的
			
			//可以修改延时值，查看CPU使用率的差异
			OSTimeDly(1000u, 
									OS_OPT_TIME_DLY,
									&os_err);


	}
	
}





/*********************************************END OF FILE**********************/

