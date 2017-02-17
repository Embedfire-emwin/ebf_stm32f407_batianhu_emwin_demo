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
#include "GUICSV_Port.h"   
#include "ff.h"

/*字库存储路径*/
#if (CSV_SOURCE == USE_FLASH_CSV)

	static const uint32_t  CSV_XINSONGTI_25_ADDR	 = (30*4096);
	static const uint32_t  CSV_XINSONGTI_19_ADDR	 = (680*4096);

#elif (CSV_SOURCE == USE_SDCARD_CSV)

	static const char  CSV_STORAGE_ROOT_DIR[] = "0:";
	static const char LANG_CSV_ADDR[] = 	 "0:srcdata/LANG_CSV.csv";
	static const char CSV_XINSONGTI_19_ADDR[] = 	 "0:srcdata/新宋体19.xbf";

#elif (CSV_SOURCE == USE_FLASH_FILESYSTEM_CSV)
	
	static const char CSV_STORAGE_ROOT_DIR[] =  		"1:";
	static const char CSV_XINSONGTI_25_ADDR[] = 	 "1:新宋体25.xbf";
	static const char CSV_XINSONGTI_19_ADDR[] = 	 "1:新宋体19.xbf";

#endif


/*字库结构体*/
//GUI_XBF_DATA 	XBF_XINSONGTI_25_Data;
//GUI_FONT     	CSV_XINSONGTI_25;

//GUI_XBF_DATA 	XBF_XINSONGTI_19_Data;
//GUI_FONT     	CSV_XINSONGTI_19;

/*存储器初始化标志*/
static uint8_t storage_init_flag = 0;


/*字库存储在文件系统时需要使用的变量*/
#if (CSV_SOURCE == USE_SDCARD_CSV || CSV_SOURCE == USE_FLASH_FILESYSTEM_CSV)
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
// int _cb_CSV_GetData( void * pVoid, const U8 ** ppData, unsigned NumBytes, U32 Off)
//{
//#if (CSV_SOURCE == USE_FLASH_CSV)
//	
//	if (storage_init_flag == 0)
//	{
//		//初始化flash
//		TM_FATFS_FLASH_SPI_disk_initialize();
//		storage_init_flag = 1;
//	}
//	
//	//从pVoid中获取字库的存储地址(pvoid的值在GUI_XBF_CreateFont中传入)
//	SPI_FLASH_BufferRead(pBuffer,*(uint32_t *)pVoid+Offset,NumBytes);
//	
//	return 0;
//	
//#elif (CSV_SOURCE == USE_SDCARD_CSV || CSV_SOURCE == USE_FLASH_FILESYSTEM_CSV)

//	if (storage_init_flag == 0)
//	{
//		//挂载sd卡文件系统
//		res = f_mount(&fs,CSV_STORAGE_ROOT_DIR,1);
//		storage_init_flag = 1;
//	}
//	
//	//从pVoid中获取字库的存储地址(pvoid的值在GUI_XBF_CreateFont中传入)
//	res = f_open(&fnew , (char *)pVoid, FA_OPEN_EXISTING | FA_READ);

//	if ( res == FR_OK ) 
//	{
//			f_lseek (&fnew, Off);		//指针偏移
//		
//			//读取内容
//			res = f_read( &fnew, ppData, NumBytes, &br );		 
//			
//			f_close(&fnew);
//			
//			return 0;  
//	}    
//	else
//			return 1; 

//#endif

//}


/**
  * @brief  PNG/BITMAPS获取外部图片文件数据的回调函数，作为GUI_xx_DrawEx的回调函数
  * @param  p: 外部调用时输入的指针，一般使用文件指针.
  * @param  NumBytesReq:请求的字节数。
  * @param 	ppData：获取到的数据要填充的指针。必须使用 “GetData”函数填充指针指向的位置。
  * @param  Off ：如果此标记为 1，应将数据指针设置到数据流的起始位置
  * @retval 返回可用的数据字节数
  */
static int _cb_CSV_GetData(void * p, const U8 **ppData, unsigned int NumBytesReq, U32 Off) 
{
	
	UINT    NumBytesRead;
	U8     * pData;
  FIL *p_file;
	FRESULT result;  

	p_file=(FIL *)p;
	pData  = (U8 *)*ppData;
	
	
	result =f_lseek(p_file, Off);
	if (result != FR_OK)
	{
		return 0;
	}
//  printf("lseek:result->(%d) off->(%ld) address->(%d)\n",result,Off,FileAddress);
	/*
	* 读取数据到指针的位置
	*/
	result = f_read(p_file, pData, NumBytesReq, &NumBytesRead);
//  printf("read:result->(%d) numreq->(%d),numread->(%d)\n",result,NumBytesReq,NumBytesRead);
	/*
	* 返回读取到的有效字节数
	*/
	return NumBytesRead;
}

  U8           NumLanguagesCSV;
static	FIL file;
static FATFS fs;													/* Work area (file system object) for logical drives */

 /**
  * @brief  创建XBF字体
  * @param  无
  * @retval 无
  */
void Create_LANG_CSV(void) 
{	
	FRESULT result;  

	const char *readText;
	
	f_mount(&fs,"0:",1);	

	
	result = f_open(&file, LANG_CSV_ADDR, FA_OPEN_EXISTING | FA_READ );

	
	NumLanguagesCSV = GUI_LANG_LoadCSVEx(_cb_CSV_GetData, &file);




	
}



/*********************************************END OF FILE**********************/
