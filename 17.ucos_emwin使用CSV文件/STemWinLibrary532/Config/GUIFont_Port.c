/**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   XBF格式字体emwin函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include "./fatfs/drivers/fatfs_flash_spi.h"
#include "GUIFont_Port.h"   
#include "ff.h"

/*字库存储路径*/
#if (XBF_FONT_SOURCE == USE_FLASH_FONT)

	static const uint32_t  FONT_XINSONGTI_25_ADDR	 = (30*4096);
	static const uint32_t  FONT_XINSONGTI_19_ADDR	 = (680*4096);

#elif (XBF_FONT_SOURCE == USE_SDCARD_FONT)

	static const char  FONT_STORAGE_ROOT_DIR[] = "0:";
	static const char FONT_XINSONGTI_25_ADDR[] = 	 "0:srcdata/新宋体25.xbf";
	static const char FONT_XINSONGTI_19_ADDR[] = 	 "0:srcdata/新宋体19.xbf";

#elif (XBF_FONT_SOURCE == USE_FLASH_FILESYSTEM_FONT)
	
	static const char FONT_STORAGE_ROOT_DIR[] =  		"1:";
	static const char FONT_XINSONGTI_25_ADDR[] = 	 "1:新宋体25.xbf";
	static const char FONT_XINSONGTI_19_ADDR[] = 	 "1:新宋体19.xbf";

#endif


/*字库结构体*/
GUI_XBF_DATA 	XBF_XINSONGTI_25_Data;
GUI_FONT     	FONT_XINSONGTI_25;

GUI_XBF_DATA 	XBF_XINSONGTI_19_Data;
GUI_FONT     	FONT_XINSONGTI_19;

/*存储器初始化标志*/
static uint8_t storage_init_flag = 0;


/*字库存储在文件系统时需要使用的变量*/
#if (XBF_FONT_SOURCE == USE_SDCARD_FONT || XBF_FONT_SOURCE == USE_FLASH_FILESYSTEM_FONT)
	static FIL fnew;										/* file objects */
	static FATFS fs;										/* Work area (file system object) for logical drives */
	static FRESULT res; 
	static UINT br;            				/* File R/W count */
#endif

 /**
  * @brief  获取字体数据的回调函数
  * @param  Offset：要读取的内容在XBF文件中的偏移位置
  * @param  NumBytes：要读取的字节数
	* @param  pVoid：自定义数据的指针
  * @param  pBuffer：存储读取内容的指针
  * @retval 0 成功, 1 失败
  */
static int _cb_FONT_XBF_GetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
#if (XBF_FONT_SOURCE == USE_FLASH_FONT)
	
	if (storage_init_flag == 0)
	{
		//初始化flash
		TM_FATFS_FLASH_SPI_disk_initialize();
		storage_init_flag = 1;
	}
	
	//从pVoid中获取字库的存储地址(pvoid的值在GUI_XBF_CreateFont中传入)
	SPI_FLASH_BufferRead(pBuffer,*(uint32_t *)pVoid+Offset,NumBytes);
	
	return 0;
	
#elif (XBF_FONT_SOURCE == USE_SDCARD_FONT || XBF_FONT_SOURCE == USE_FLASH_FILESYSTEM_FONT)

	if (storage_init_flag == 0)
	{
		//挂载sd卡文件系统
		res = f_mount(&fs,FONT_STORAGE_ROOT_DIR,1);
		storage_init_flag = 1;
	}
	
	//从pVoid中获取字库的存储地址(pvoid的值在GUI_XBF_CreateFont中传入)
	res = f_open(&fnew , (char *)pVoid, FA_OPEN_EXISTING | FA_READ);

	if ( res == FR_OK ) 
	{
			f_lseek (&fnew, Offset);		//指针偏移
		
			//读取内容
			res = f_read( &fnew, pBuffer, NumBytes, &br );		 
			
			f_close(&fnew);
			
			return 0;  
	}    
	else
			return 1; 

#endif

}




 /**
  * @brief  创建XBF字体
  * @param  无
  * @retval 无
  */
void Creat_XBF_Font(void) 
{	
	//
	// 新宋体25
	//
	GUI_XBF_CreateFont(&FONT_XINSONGTI_25,    					// GUI_FONT 字体结构体指针
					   &XBF_XINSONGTI_25_Data,         						// GUI_XBF_DATA 结构体指针
					   GUI_XBF_TYPE_PROP_EXT, 										// 字体类型
					   _cb_FONT_XBF_GetData,  							// 获取字体数据的回调函数
					   (void *)&FONT_XINSONGTI_25_ADDR);        	// 要传输给回调函数的自定义数据指针，此处传输字库的地址
	//
	// 新宋体19
	//
	GUI_XBF_CreateFont(&FONT_XINSONGTI_19,    				// GUI_FONT 字体结构体指针
					 &XBF_XINSONGTI_19_Data,         						// GUI_XBF_DATA 结构体指针
					 GUI_XBF_TYPE_PROP_EXT, 										// 字体类型
					 _cb_FONT_XBF_GetData,  							// 获取字体数据的回调函数
					 (void *)&FONT_XINSONGTI_19_ADDR);        	// 要传输给回调函数的自定义数据指针，此处传输字库的地址

}


 /**
  * @brief  GBK转UTF8
  * @param  src：输入的字符串（GBK格式）
  * @param  str：输出的字符串（UTF8格式）
  * @retval 无
  */
void COM_gbk2utf8(const char *src, char *str)
{
	uint32_t j=0,k=0;
	uint16_t gbkdata=0;
	uint16_t UCbuffer[512]={0};
	for(j=0,k=0;src[j]!='\0';k++)
	{
		if((uint8_t)src[j]>0x80)
		{
			gbkdata=src[j+1]+src[j]*256;
			UCbuffer[k]=ff_convert(gbkdata,1);
			j+=2;
		}
		else
		{
			UCbuffer[k]=0x00ff&src[j];
			j+=1;
		}
	}
	UCbuffer[k]='\0';
	GUI_UC_ConvertUC2UTF8(UCbuffer,2*k+2,str,k*3);
	str[k*3]=0;
}

/*********************************************END OF FILE**********************/
