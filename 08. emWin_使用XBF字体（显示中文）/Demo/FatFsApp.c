/**
  ******************************************************************************
  * @file    GUIDEMO_Fatfs.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试文件系统
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F407 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "stm32f4xx.h"
#include "./usart/bsp_debug_usart.h"
#include "ff.h"
#include "GUI.h"
#include "GUIFont_Port.h"   


FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[4096]={0};       		  /* file copy buffer */
char utf8_buffer[4096*2]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "欢迎使用野火F407开发板，这是一个使用XBF字体显示的例子，本例子支持中文显示。";

//要测试的磁盘号，0为spi，1为sd卡
char disk_num[] = "0:";



void Fatfs_MainTask(void)
{
		char Str_temp[100];
		 
		/* 用于显示文件内容的窗口大小 */ 
		GUI_RECT file_content;
	
		//选择字体
		GUI_SetFont(&FONT_XINSONGTI_19);
	
		file_content.x0 = 5;
		file_content.y0 =GUI_GetFontSizeY() * 7;
		file_content.x1 = LCD_GetXSize()-5;
		file_content.y1 = LCD_GetYSize()-5;
	
		/* 给屏幕刷上背景颜色 */
		GUI_Clear();	
	  
		//文字转换成UTF8格式
		COM_gbk2utf8("挂载文件系统...",utf8_buffer);

		GUI_DispStringAt (utf8_buffer,5,GUI_GetFontSizeY()*1);
		
		GUI_Delay(500);		
		
		/* Register work area for each volume (Always succeeds regardless of disk status) */
		res = f_mount(&fs,disk_num,1);	
		
		if(res == FR_NO_FILESYSTEM)
		{
			/* 设置前景颜色（字体颜色）*/
			GUI_SetColor(GUI_RED);

			//文字转换成UTF8格式
			COM_gbk2utf8("存储器没有文件系统，需要格式化！",utf8_buffer);

			GUI_DispStringAt (utf8_buffer,5,GUI_GetFontSizeY()*3);
			while(1);
		}
		else if	(res != FR_OK)
		{
			/* 设置前景颜色（字体颜色）*/
			GUI_SetColor(GUI_RED);
			
			sprintf(Str_temp,"文件系统 错误:%d !",res);
			
			//文字转换成UTF8格式
			COM_gbk2utf8(Str_temp,utf8_buffer);

			GUI_DispStringAt (utf8_buffer,5,GUI_GetFontSizeY()*3);
			while(1);
		}

		/* function disk_initialize() has been called in f_open */
		
	  GUI_DispStringAt ("f_open :newfile.txt ...",5,GUI_GetFontSizeY()*2);
	
		//拼接文件路径
		sprintf(Str_temp,"%snewfile.txt",disk_num);
		/* Create new file on the drive  */
		res = f_open(&fnew, Str_temp, FA_CREATE_ALWAYS | FA_WRITE );
 
		if ( res == FR_OK )
		{
			res = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
			f_close(&fnew);      
		}
		else
		{
			/* 设置前景颜色（字体颜色）*/
			GUI_SetColor(GUI_RED);
			
			sprintf(Str_temp,"文件系统 错误:%d !",res);
			
			//文字转换成UTF8格式
			COM_gbk2utf8(Str_temp,utf8_buffer);

			GUI_DispStringAt (utf8_buffer,5,GUI_GetFontSizeY()*5);
			while(1);
		}

		GUI_DispStringAt ("f_read :newfile.txt ...",5,GUI_GetFontSizeY()*3);
		
		res = f_open(&fnew, Str_temp, FA_OPEN_EXISTING | FA_READ); 	 
		res = f_read(&fnew, buffer, sizeof(buffer), &br); 

		printf("\r\n %s ", buffer);
				
		//文字转换成UTF8格式
		COM_gbk2utf8("文件内容：",utf8_buffer);

		GUI_DispStringAt (utf8_buffer,5,GUI_GetFontSizeY()*5);		
		
		/* 设置前景颜色（字体颜色）*/
		GUI_SetColor(GUI_RED);
		
		/* 设置字体 */
		GUI_SetFont(&FONT_XINSONGTI_25);
		
		//把读取到的文字转换成UTF8格式
		COM_gbk2utf8((char*)buffer,utf8_buffer);

		/* 显示文本到屏幕上 */
		GUI_DispStringInRectWrap((const char *)utf8_buffer,&file_content,GUI_TA_LEFT,GUI_WRAPMODE_WORD) ;
		
		/* Close open files */
		f_close(&fnew);	                                      
		 
    /* Unregister work area prior to discard it */
		f_mount(NULL,disk_num,1);	
		
		
				//把读取到的文字转换成UTF8格式
		COM_gbk2utf8((char*)buffer,(char*)utf8_buffer);

		/* 显示文本到屏幕上 */
		GUI_DispStringInRectWrap((const char *)utf8_buffer,&file_content,GUI_TA_LEFT,GUI_WRAPMODE_WORD) ;

		
		while(1);
}

/*********************************************END OF FILE**********************/

