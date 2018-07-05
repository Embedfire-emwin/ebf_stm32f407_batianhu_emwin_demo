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
  * 实验平台:野火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./usart/bsp_debug_usart.h"

#include "GUIPicture_Port.h"   
#include "ff.h"

//传给getData函数的指针
typedef struct
{
	FIL *file;
	uint8_t *_acBuffer;
}IN_DATA;


 /**
  * @brief  BMP/GIF/JPEG获取外部图片文件数据的回调函数，作为GUI_xx_DrawEx的回调函数
  * @param  p: 外部调用时输入的指针，一般使用文件指针.
  * @param  NumBytesReq:请求的字节数。
  * @param 	ppData：指向到获取到数据的指针。“GetData”函数必须将指针设置到请求数据所在的有效位置。
  * @param  Off ：如果此标记为 1，应将数据指针设置到数据流的起始位置
  * @retval 返回可用的数据字节数
  */
static int _GetData_BMP_GIF_JPEG(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) 
{

	int FileAddress = 0;
	UINT NumBytesRead;
	IN_DATA *in_data;
	FRESULT result;  

	
	in_data = (IN_DATA *)p;
	
	/*
	* 检查Buffer大小
	*/
	if (NumBytesReq > PIC_BUFFRE_SIZE) {
	NumBytesReq = PIC_BUFFRE_SIZE;
	}

	/*
	* 根据Off参数设置位置
	*/
	if(Off == 1) 
		FileAddress = 0;
	else 
		FileAddress = Off;
	
	//对文件进行定位
	result =f_lseek(in_data->file, FileAddress);
	if (result != FR_OK)
	{
		return 0;
	}

//printf("lseek:result->(%d) off->(%ld) address->(%d)\n",result,Off,FileAddress);
	/*
	* 读取数据到Buffer
	*/
	result = f_read(in_data->file, in_data->_acBuffer, NumBytesReq, &NumBytesRead);
//printf("read:result->(%d) numreq->(%d),numread->(%d)\n",result,NumBytesReq,NumBytesRead);
	/*
	* 设置指针指向缓冲区
	*/
	*ppData = (const U8 *)in_data->_acBuffer;

	/*
	* 返回读取到的有效字节数
	*/
	return NumBytesRead;
}


 /**
  * @brief  PNG/BITMAPS获取外部图片文件数据的回调函数，作为GUI_xx_DrawEx的回调函数
  * @param  p: 外部调用时输入的指针，一般使用文件指针.
  * @param  NumBytesReq:请求的字节数。
  * @param 	ppData：获取到的数据要填充的指针。必须使用 “GetData”函数填充指针指向的位置。
  * @param  Off ：如果此标记为 1，应将数据指针设置到数据流的起始位置
  * @retval 返回可用的数据字节数
  */
static int _GetData_PNG_BITMAPS(void * p, const U8 **ppData, unsigned int NumBytesReq, U32 Off) {
	
	int FileAddress = 0;
	UINT    NumBytesRead;
	U8     * pData;
  FIL *p_file;
	FRESULT result;  

	p_file=(FIL *)p;
	pData  = (U8 *)*ppData;
	
	/*
	* 根据Off参数设置位置
	*/	
	if(Off == 1) 
		FileAddress = 0;
	else 
		FileAddress = Off;
	
	result =f_lseek(p_file, FileAddress);
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

 /**
  * @brief  显示文件系统中的BMP图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @retval 无
  */
void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) 
{	 
	FRESULT result;  

	IN_DATA in_data;
	
	GUI_HMEM hMem,hMem2;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(PIC_BUFFRE_SIZE);
	//把空间句柄转成指针
	in_data._acBuffer = GUI_ALLOC_h2p(hMem);
	
	//从emwin内存中分配空间
	hMem2 = GUI_ALLOC_AllocZero(sizeof(FIL));

	//把空间句柄转成指针
	in_data.file = GUI_ALLOC_h2p(hMem2);
	
    /* 打开文件 */		
	result = f_open(in_data.file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
  
	if (result != FR_OK)
	{
		return;
	}

  printf("file open ok\n");
	
	GUI_BMP_DrawEx(_GetData_BMP_GIF_JPEG, &in_data, usPOSX, usPOSY);

	f_close(in_data.file);
	
	//释放申请的空间
	GUI_ALLOC_Free(hMem);
		//释放申请的空间
	GUI_ALLOC_Free(hMem2);

}

 /**
  * @brief  显示文件系统中的JPEG图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @retval 无
  */
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY)  
{	 
	FRESULT result;  

	IN_DATA in_data;
	
	GUI_HMEM hMem,hMem2;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(PIC_BUFFRE_SIZE);
	//把空间句柄转成指针
	in_data._acBuffer = GUI_ALLOC_h2p(hMem);
	
	//从emwin内存中分配空间
	hMem2 = GUI_ALLOC_AllocZero(sizeof(FIL));

	//把空间句柄转成指针
	in_data.file = GUI_ALLOC_h2p(hMem2);

	
	/* 打开文件 */		
	result = f_open(in_data.file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		printf("result = %d",result);
		return;
	}

  GUI_JPEG_DrawEx(_GetData_BMP_GIF_JPEG, &in_data, usPOSX, usPOSY);

	f_close(in_data.file);
	

	//释放申请的空间
	GUI_ALLOC_Free(hMem);
		//释放申请的空间
	GUI_ALLOC_Free(hMem2);

}


/**
  * @brief  显示内部的GIF图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @param 	ctime：GIF图片循环显示多少次，
                   若输入ctime为0,则一直循环显示GIF图片，本函数内部会死循环
  * @retval 无
  */
void _ShowGIF(uint8_t * imgBuffer, uint32_t BufferSize,uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  
{	   
 	GUI_GIF_INFO 				GIFInfo;
	GUI_GIF_IMAGE_INFO  GIF_ImageInfo;
	uint16_t		i=0;
	uint32_t   t0,t1;
	uint8_t 		forver =  0;
	
	//若输入的ctime == 0，GIF图片一直循环显示
	if(ctime == 0) forver =1;	

	//获取GIF图片信息
	GUI_GIF_GetInfo(imgBuffer,BufferSize,&GIFInfo);

	while(ctime-- || forver)
	{
		i=0;
		//循环显示所有子图片
		while(i<GIFInfo.NumImages)
		{
			GUI_GIF_GetImageInfo(imgBuffer,BufferSize,&GIF_ImageInfo,i);
		
			//emwin要求，当GIF_ImageInfo.Delay ==0 时，要延时100ms
			if(GIF_ImageInfo.Delay == 0)
				GUI_Delay(100);
			
			t0 = GUI_GetTime();
			GUI_GIF_DrawSub(imgBuffer,
													BufferSize, 
													usPOSX, 
													usPOSY,
													i++);
			//计算解码本身消耗的时间
			t1 = GUI_GetTime() - t0;
			
			//GIF_ImageInfo.Delay的单位为10ms，所以乘以10
			if(t1 < GIF_ImageInfo.Delay*10)
			{
				GUI_Delay(GIF_ImageInfo.Delay*10 - t1);
			}			
		}	
	}

}

 /**
  * @brief  显示文件系统中的GIF图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @param 	ctime：GIF图片循环显示多少次，
                   若输入ctime为0,则一直循环显示GIF图片，本函数内部会死循环
  * @retval 无
  */
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  
{	  
	FRESULT result;  	
 	GUI_GIF_INFO 				GIFInfo;
	GUI_GIF_IMAGE_INFO  GIF_ImageInfo;
	uint16_t		i=0;
	uint32_t   t0,t1;
	uint8_t 		forver =  0;
		
	IN_DATA in_data;
	
	GUI_HMEM hMem,hMem2;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(PIC_BUFFRE_SIZE);
	//把空间句柄转成指针
	in_data._acBuffer = GUI_ALLOC_h2p(hMem);
	
	//从emwin内存中分配空间
	hMem2 = GUI_ALLOC_AllocZero(sizeof(FIL));

	//把空间句柄转成指针
	in_data.file = GUI_ALLOC_h2p(hMem2);

	
	//若输入的ctime == 0，GIF图片一直循环显示
	if(ctime == 0) forver =1;	

	/* 打开文件 */		
	result = f_open(in_data.file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		return;
	}
	

	//获取GIF图片信息
	GUI_GIF_GetInfoEx(_GetData_BMP_GIF_JPEG,&in_data,&GIFInfo);

	while(ctime-- || forver)
	{
		i=0;
		//循环显示所有子图片
		while(i<GIFInfo.NumImages)
		{
			GUI_GIF_GetImageInfoEx(_GetData_BMP_GIF_JPEG,&in_data,&GIF_ImageInfo,i);
		
			//emwin要求，当GIF_ImageInfo.Delay ==0 时，要延时100ms
			if(GIF_ImageInfo.Delay == 0)
				GUI_Delay(100);
			
			t0 = GUI_GetTime();
			GUI_GIF_DrawSubEx(_GetData_BMP_GIF_JPEG,
													&in_data, 
													usPOSX, 
													usPOSY,
													i++);
			//计算解码本身消耗的时间
			t1 = GUI_GetTime()-t0;
			
			//GIF_ImageInfo.Delay的单位为10ms，所以乘以10
			if(t1 < GIF_ImageInfo.Delay*10)
			{
				GUI_Delay(GIF_ImageInfo.Delay*10-t1);
			}			
		}	
	}

	f_close(in_data.file);

	//释放申请的空间
	GUI_ALLOC_Free(hMem);
		//释放申请的空间
	GUI_ALLOC_Free(hMem2);

}

 /**
  * @brief  显示文件系统中的PNG图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @retval 无
  */
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) 
{  
	FRESULT result;  

	FIL *file;
	
	GUI_HMEM hMem;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(sizeof(FIL));

	//把空间句柄转成指针
	file = GUI_ALLOC_h2p(hMem);

	
	
/* 打开文件 */		
	result = f_open(file, sFilename, FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
//  printf("\nopen file (%s) result->(%d)\n",sFilename,result);
	if (result != FR_OK)
	{
		return;
	}  
	GUI_PNG_DrawEx(_GetData_PNG_BITMAPS, file, usPOSX, usPOSY); 
//  printf("png draw ok\n");
	f_close(file);		

	//释放申请的空间
	GUI_ALLOC_Free(hMem);


}
/*********************************************END OF FILE**********************/
