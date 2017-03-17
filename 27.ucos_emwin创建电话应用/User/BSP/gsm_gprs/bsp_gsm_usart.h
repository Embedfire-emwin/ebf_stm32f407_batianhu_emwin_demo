#ifndef __BSP_GSM_USART_H
#define	__BSP_GSM_USART_H

#include "stm32f4xx.h"
#include <stdio.h>


// GSM_GPRS使用的串口
#define  GSM_USARTx                   USART2
#define  GSM_USART_CLK                RCC_APB1Periph_USART2
#define  GSM_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  GSM_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  GSM_USART_GPIO_CLK           (RCC_AHB1Periph_GPIOA)
    
#define  GSM_TX_GPIO_PORT         			GPIOA   
#define  GSM_TX_GPIO_PIN          			GPIO_Pin_2
#define 	GSM_TX_AF             					GPIO_AF_USART2
#define 	GSM_TX_SOURCE         					GPIO_PinSource2

#define  GSM_RX_GPIO_PORT       				GPIOA
#define  GSM_RX_GPIO_PIN        				GPIO_Pin_3
#define 	GSM_RX_AF             					GPIO_AF_USART2
#define 	GSM_RX_SOURCE         					GPIO_PinSource3


#define  GSM_USART_IRQ                USART2_IRQn
#define  GSM_USART_IRQHandler         USART2_IRQHandler

void GSM_USART_Config(void);
void GSM_USART_Stop(void);


void bsp_GSM_USART_IRQHandler(void);
char *get_rebuff(uint8_t *len);
void clean_rebuff(void);

void GSM_USART_printf(char *Data,...);

#endif /* __BSP_GSM_USART_H */
