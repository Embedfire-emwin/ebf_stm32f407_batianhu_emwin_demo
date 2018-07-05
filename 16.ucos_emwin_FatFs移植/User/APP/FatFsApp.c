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

#include "includes.h"

FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[4096]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "Welcome to use Wildfire F407 stm32 Development Board,\
this is a demo about enWin && Flash FatFS.\n\
today is a good day";

//要测试的磁盘号，0为sd卡，1为spi
char disk_num[] = "1:";



void Fatfs_MainTask(void)
{
		char Str_temp[100];
		OS_ERR  os_err;

		 
		/* 用于显示文件内容的窗口大小 */ 
		GUI_RECT file_content;
	
		GUI_SetFont(GUI_FONT_8X16X2X2);
	
		file_content.x0 = 5;
		file_content.y0 =GUI_GetFontSizeY() * 7;
		file_content.x1 = LCD_GetXSize()-5;
		file_content.y1 = LCD_GetYSize()-5;
	
		/* 给屏幕刷上背景颜色 */
		GUI_Clear();	
	  
		GUI_DispStringAt ("f_mount...",5,GUI_GetFontSizeY()*1);
		
		GUI_Delay(500);		
		
		/* Register work area for each volume (Always succeeds regardless of disk status) */
		res = f_mount(&fs,disk_num,1);	
		
		if(res == FR_NO_FILESYSTEM)
		{
			/* 设置前景颜色（字体颜色）*/
			GUI_SetColor(GUI_RED);

			GUI_DispStringAt ("File System not found on disk !",5,GUI_GetFontSizeY()*3);
			while(1);
		}
		else if	(res != FR_OK)
		{
			/* 设置前景颜色（字体颜色）*/
			GUI_SetColor(GUI_RED);
			
			sprintf(Str_temp,"File System err:%d !",res);

			GUI_DispStringAt (Str_temp,5,GUI_GetFontSizeY()*3);
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
			
			sprintf(Str_temp,"File System err:%d !",res);

			GUI_DispStringAt (Str_temp,5,GUI_GetFontSizeY()*5);
			while(1);
		}

		GUI_DispStringAt ("f_read :newfile.txt ...",5,GUI_GetFontSizeY()*3);
		
		res = f_open(&fnew, Str_temp, FA_OPEN_EXISTING | FA_READ); 	 
		res = f_read(&fnew, buffer, sizeof(buffer), &br); 

		printf("\r\n %s ", buffer);
		
		GUI_DispStringAt ("file content:",5,GUI_GetFontSizeY()*5);
		
		/* 设置前景颜色（字体颜色）*/
		GUI_SetColor(GUI_RED);
		
		/* 设置字体 */
		GUI_SetFont(GUI_FONT_COMIC24B_ASCII);
		
		/* 显示文本到屏幕上 */
		GUI_DispStringInRectWrap((const char *)buffer,&file_content,GUI_TA_LEFT,GUI_WRAPMODE_WORD) ;
		
		/* Close open files */
		f_close(&fnew);	 

		while(1)	
		{			
			//延时，所有任务函数的死循环内都应有至少1ms延时
		  //特别是高优先级的任务，若无延时，其它低优先级任务可能会无机会执行
			OSTimeDly(1000u, 
									OS_OPT_TIME_DLY,
									&os_err);
		}
}

/*********************************************END OF FILE**********************/

