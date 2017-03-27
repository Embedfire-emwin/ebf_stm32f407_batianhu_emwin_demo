#ifndef __GUI_PICTURE_PORT_H
#define	__GUI_PICTURE_PORT_H

#include "stm32f4xx.h"
#include "GUI.h"

/* 实际的测试需要是图像宽度的4倍即可，切记(也就是保证每个像素如果是32位数据的情况) */
#define PIC_BUFFRE_SIZE  (800*4)


void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ;
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) ; 
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime);  
void _ShowGIF(uint8_t * imgBuffer, uint32_t BufferSize,uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  ;
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY); 

#endif /* __GUI_PICTURE_PORT_H */
