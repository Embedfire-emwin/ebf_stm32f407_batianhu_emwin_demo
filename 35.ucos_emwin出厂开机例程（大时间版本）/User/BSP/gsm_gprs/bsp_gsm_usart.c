/**
  ******************************************************************************
  * @file    bsp_gsm_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   GSM模块的串口驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "./gsm_gprs/bsp_gsm_usart.h"
#include <stdarg.h>


/// 配置USART接收中断
static void GSM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = GSM_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：GSM_USART_Config
 * 描述  ：GSM_USART GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void GSM_USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config GSM_USARTx clock */
	RCC_AHB1PeriphClockCmd(GSM_USART_GPIO_CLK, ENABLE); 
	GSM_USART_APBxClkCmd(GSM_USART_CLK, ENABLE);
	
	  /* 连接 PXx 到 USARTx_Tx*/
  GPIO_PinAFConfig(GSM_RX_GPIO_PORT,GSM_RX_SOURCE, GSM_RX_AF);

  /*  连接 PXx 到 USARTx__Rx*/
  GPIO_PinAFConfig(GSM_TX_GPIO_PORT,GSM_TX_SOURCE,GSM_TX_AF);

  /* 配置Tx引脚为复用功能  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

  GPIO_InitStructure.GPIO_Pin = GSM_TX_GPIO_PIN  ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GSM_RX_GPIO_PORT, &GPIO_InitStructure);

  /* 配置Rx引脚为复用功能 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GSM_RX_GPIO_PIN;
  GPIO_Init(GSM_RX_GPIO_PORT, &GPIO_InitStructure);

	  
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = GSM_USART_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(GSM_USARTx, &USART_InitStructure); 
	
	/*	配置中断优先级 */
	GSM_NVIC_Config();
	/* 使能串口2接收中断 */
	USART_ITConfig(GSM_USARTx, USART_IT_RXNE, ENABLE);

	
	USART_Cmd(GSM_USARTx, ENABLE);
}

/*
 * 函数名：GSM_USART_Stop
 * 描述  ：关闭GSM模块使用的串口
 */
void GSM_USART_Stop(void)
{
    USART_ITConfig(GSM_USARTx, USART_IT_RXNE, DISABLE);	
    USART_Cmd(GSM_USARTx, DISABLE);
		GSM_USART_APBxClkCmd(GSM_USART_CLK, DISABLE);
}

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到GSM_USARTx
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
//int fputc(int ch, FILE *f)
//{
///* 将Printf内容发往串口 */
//  USART_SendData(GSM_USARTx, (unsigned char) ch);
//  while (!(GSM_USARTx->SR & USART_FLAG_TXE));
// 
//  return (ch);
//}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被GSM_USARTx_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


#if 1
//中断缓存串口数据
#define UART_BUFF_SIZE      255
volatile    uint8_t uart_p = 0;
uint8_t     uart_buff[UART_BUFF_SIZE];

void bsp_GSM_USART_IRQHandler(void)
{
    if(uart_p<UART_BUFF_SIZE)
    {
        if(USART_GetITStatus(GSM_USARTx, USART_IT_RXNE) != RESET)
        {
            uart_buff[uart_p] = USART_ReceiveData(GSM_USARTx);
            uart_p++;
        }
    }
}



//获取接收到的数据和长度
char *get_rebuff(uint8_t *len)
{
    *len = uart_p;
    return (char *)&uart_buff;
}

void clean_rebuff(void)
{
	uint16_t i=UART_BUFF_SIZE+1;
    uart_p = 0;
	while(i)
		uart_buff[--i]=0;
}

#endif

/*
 * 函数名：GSM_USARTx_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即GSM_USARTx
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用GSM_USARTx_printf( GSM_USARTx, "\r\n this is a demo \r\n" );
 *            		 GSM_USARTx_printf( GSM_USARTx, "\r\n %d \r\n", i );
 *            		 GSM_USARTx_printf( GSM_USARTx, "\r\n %s \r\n", j );
 */
void GSM_USART_printf(char *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(GSM_USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(GSM_USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(GSM_USARTx,*s);
						while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(GSM_USARTx,*s);
						while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(GSM_USARTx, *Data++);
		while( USART_GetFlagStatus(GSM_USARTx, USART_FLAG_TXE) == RESET );
	}
}




/********中断服务函数*****************/
//GSM串口的中断服务函数
void GSM_USART_IRQHandler(void)
{

    bsp_GSM_USART_IRQHandler();

}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
