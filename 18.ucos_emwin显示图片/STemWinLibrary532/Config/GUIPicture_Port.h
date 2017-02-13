#ifndef __GUI_PICTURE_PORT_H
#define	__GUI_PICTURE_PORT_H

#include "stm32f4xx.h"
#include "GUI.h"

//选择使用emwin的内存空间（动态分配）还是静态空间（定义成全局变量）
#define USE_MEM 			EMWIN_MEM

#define EMWIN_MEM 	 0	//emwin的内存空间
#define STATIC_MEM	 1	//静态的空间

void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ; 
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime);  
void _ShowGIF(uint8_t * imgBuffer, uint32_t BufferSize,uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  ;
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 

#endif /* __GUI_PICTURE_PORT_H */
