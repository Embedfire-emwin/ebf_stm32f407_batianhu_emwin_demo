#ifndef __BSP_GSM_GPRS_H
#define __BSP_GSM_GPRS_H

#include "stm32f4xx.h"
//#include "./systick/bsp_SysTick.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "GUI.h"

#include "ff.h"

typedef enum{
    GSM_TRUE,
    GSM_FALSE,
    
}gsm_res_e;

typedef enum
{
    GSM_NULL                = 0,
    GSM_CMD_SEND            = '\r',         
    GSM_DATA_SEND           = 0x1A,         //发送数据(ctrl + z)
    GSM_DATA_CANCLE         = 0x1B,         //发送数据(Esc)    
}gsm_cmd_end_e;

//                  指令             正常返回
//本机号码          AT+CNUM\r         +CNUM: "","13265002063",129,7,4            //很多SIM卡默认都是没设置本机号码的，解决方法如下 http://www.multisilicon.com/blog/a21234642.html
//SIM营运商         AT+COPS?\r        +COPS: 0,0,"CHN-UNICOM"   OK
//SIM卡状态         AT+CPIN?\r        +CPIN: READY   OK
//SIM卡信号强度     AT+CSQ\r          +CSQ: 8,0   OK

extern  uint8_t     gsm_cmd         	(char *cmd, char *reply,uint32_t waittime );
extern  uint8_t     gsm_cmd_check   	(char *reply);


#define     GSM_CLEAN_RX()              clean_rebuff()
#define     gsm_ate0()                  gsm_cmd("ATE0\r","OK",100)              //关闭回显
#define     GSM_TX(cmd)                	GSM_USART_printf("%s",cmd)
#define     GSM_IS_RX()                 (USART_GetFlagStatus(GSM_USARTx, USART_FLAG_RXNE) != RESET)
#define     GSM_RX(len)                 ((char *)get_rebuff(&(len)))
#define     GSM_DELAY(time)             	GUI_Delay(time) ;               //延时
#define     GSM_SWAP16(data)    				 __REVSH(data)

/*************************** 电话 功能 ***************************/
#define  	GSM_HANGON()				GSM_TX("ATA\r");								
#define  	GSM_HANGOFF()				GSM_TX("ATH\r");	//挂断电话	

uint8_t 	gsm_init								(void);															//初始化并检测模块
uint8_t     gsm_cnum            (char *num);                        //获取本机号码
void        gsm_call           	(char *num);                        //发起拨打电话（不管接不接通）
uint8_t 	IsRing					(char *num);						//查询是否来电，并保存来电号码
uint8_t  IsInsertCard(void);
uint8_t 	IsNOCARRIER(void);

/***************************  短信功能  ****************************/
uint8_t        gsm_sms             (char *num,char *smstext);          //发送短信（支持中英文,中文为GBK码）
char *      gsm_waitask         (uint8_t waitask_hook(void));       //等待有数据应答，返回接收缓冲区地址
void        gsm_gbk2ucs2        (char * ucs2,char * gbk);           

uint8_t 	IsReceiveMS				(void);
uint8_t 	readmessage				(uint8_t messadd,char *num,char *str);
uint8_t 	hexuni2gbk				(char *hexuni,char *chgbk);

/*************************** GPRS 功能 ***************************/
uint8_t 	gsm_gprs_init		(void);																//GPRS初始化环境
uint8_t gsm_gprs_tcp_link	(char *localport,char * serverip,char * serverport);				//TCP连接
uint8_t gsm_gprs_udp_link	(char *localport,char * serverip,char * serverport);				//UDP连接
uint8_t gsm_gprs_send		(const char * str);														//发送数据
uint8_t gsm_gprs_link_close	(void);              												//IP链接断开
uint8_t gsm_gprs_shut_close	(void);               												//关闭场景
uint8_t	PostGPRS(void);





/*调试用串口*/

#define GSM_DEBUG_ON         	0
#define GSM_DEBUG_ARRAY_ON    0
#define GSM_DEBUG_FUNC_ON   	0
// Log define
#define GSM_INFO(fmt,arg...)           printf("<<-GSM-INFO->> "fmt"\n",##arg)
#define GSM_ERROR(fmt,arg...)          printf("<<-GSM-ERROR->> "fmt"\n",##arg)
#define GSM_DEBUG(fmt,arg...)          do{\
                                         if(GSM_DEBUG_ON)\
                                         printf("<<-GSM-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
																					}while(0)

#define GSM_DEBUG_ARRAY(array, num)    do{\
                                         int32_t i;\
                                         uint8_t* a = array;\
                                         if(GSM_DEBUG_ARRAY_ON)\
                                         {\
                                            printf("<<-GSM-DEBUG-ARRAY->> [%d]\n",__LINE__);\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printf("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printf("\n");\
                                                }\
                                            }\
                                            printf("\n");\
                                        }\
                                       }while(0)

#define GSM_DEBUG_FUNC()               do{\
                                         if(GSM_DEBUG_FUNC_ON)\
                                         printf("<<-GSM-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)

#endif

