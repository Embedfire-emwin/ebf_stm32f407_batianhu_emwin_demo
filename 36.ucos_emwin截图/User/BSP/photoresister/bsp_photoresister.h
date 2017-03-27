#ifndef __BSP_PHOTORESISTER_H
#define	__BSP_PHOTORESISTER_H

#include "stm32f4xx.h"

//全局变量
extern __IO uint16_t Photoresister_ADC_ConvertedValue;


// ADC GPIO 宏定义
#define PHOTORESISTER_ADC_GPIO_PORT    GPIOB
#define PHOTORESISTER_ADC_GPIO_PIN     GPIO_Pin_1
#define PHOTORESISTER_ADC_GPIO_CLK     RCC_AHB1Periph_GPIOB

// ADC 序号宏定义
#define PHOTORESISTER_ADC              ADC1
#define PHOTORESISTER_ADC_CLK          RCC_APB2Periph_ADC1
#define PHOTORESISTER_ADC_CHANNEL      ADC_Channel_9

// ADC DR寄存器宏定义，ADC转换后的数字值则存放在这里
#define PHOTORESISTER_ADC_DR_ADDR    ((u32)ADC1+0x4c)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define PHOTORESISTER_ADC_DMA_CLK      RCC_AHB1Periph_DMA2
#define PHOTORESISTER_ADC_DMA_CHANNEL  DMA_Channel_0
#define PHOTORESISTER_ADC_DMA_STREAM   DMA2_Stream0



void Photoresister_Init(void);
void Photoresister_TurnOFF(void);

#endif /* __BSP_PHOTORESISTER_H */



