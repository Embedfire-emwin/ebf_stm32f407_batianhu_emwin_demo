/**
  ******************************************************************************
  * @file    bsp_gsm_gprs.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   GSM模块驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F407 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
	*/

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "./usart/bsp_debug_usart.h"
#include "./gsm_gprs/bsp_gsm_usart.h"
#include "./gsm_gprs/bsp_gsm_gprs.h"
#include "ff.h"



static uint8_t MaxMessAdd=50;


//0表示成功，1表示失败

uint8_t gsm_cmd(char *cmd, char *reply,uint32_t waittime )
{    
		GSM_DEBUG_FUNC();
	
    GSM_CLEAN_RX();                 //清空了接收缓冲区数据

    GSM_TX(cmd);                    //发送命令

	  GSM_DEBUG("Send cmd:%s",cmd);	
	
    if(reply == 0)                      //不需要接收数据
    {
        return GSM_TRUE;
    }
    
    GSM_DELAY(waittime);                 //延时
    
    
    return gsm_cmd_check(reply);    //对接收数据进行处理
}


//0表示成功，1表示失败
uint8_t gsm_cmd_check(char *reply)
{
    uint8_t len;
    uint8_t n;
    uint8_t off;
    char *redata;
    
		GSM_DEBUG_FUNC();

    redata = GSM_RX(len);   //接收数据
   	  
	  *(redata+len) = '\0';
	  GSM_DEBUG("Reply:%s",redata);	

//		GSM_DEBUG_ARRAY((uint8_t *)redata,len);
	
    n = 0;
    off = 0;
    while((n + off)<len)
    {
        if(reply[n] == 0)                 //数据为空或者比较完毕
        {
            return GSM_TRUE;
        }
        
        if(redata[ n + off]== reply[n])
        {
            n++;                //移动到下一个接收数据
        }
        else
        {
            off++;              //进行下一轮匹配
            n=0;                //重来
        }
        //n++;
    }

    if(reply[n]==0)   //刚好匹配完毕
    {
        return GSM_TRUE;
    }
    
    return GSM_FALSE;       //跳出循环表示比较完毕后都没有相同的数据，因此跳出
}

char * gsm_waitask(uint8_t waitask_hook(void))      //等待有数据应答
{
    uint8_t len=0;
    char *redata;
	
		GSM_DEBUG_FUNC();
	
    do{
        redata = GSM_RX(len);   //接收数据
			
			
        if(waitask_hook!=0)
        {
            if(waitask_hook()==GSM_TRUE)           //返回 GSM_TRUE 表示检测到事件，需要退出
            {
                redata = 0;
                return redata;               
            }
        }
    }while(len==0);                 //接收数据为0时一直等待
    				
		GSM_DEBUG_ARRAY((uint8_t *)redata,len);

    
    GSM_DELAY(20);              //延时，确保能接收到全部数据（115200波特率下，每ms能接收11.52个字节）
    return redata;
}


//本机号码
//0表示成功，1表示失败
uint8_t gsm_cnum(char *num)
{
    char *redata;
    uint8_t len;
  
		GSM_DEBUG_FUNC();  
	
    if(gsm_cmd("AT+CNUM\r","OK", 100) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
    
    redata = GSM_RX(len);   //接收数据
    
		GSM_DEBUG_ARRAY((uint8_t *)redata,len);
		
    if(len == 0)
    {
        return GSM_FALSE;
    }
    
    //第一个逗号后面的数据为:"本机号码"
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata+=2;
    
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //字符串结尾需要清0
    return GSM_TRUE;
}


//初始化并检测模块
//0表示成功，1表示失败
uint8_t gsm_init(void)
{
	char *redata;
	uint8_t len;
	
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();                 //清空了接收缓冲区数据
	GSM_USART_Config();					//初始化串口
	
   
	if(gsm_cmd("AT+CGMM\r","OK", 100) != GSM_TRUE)
	{
			return GSM_FALSE;
	}
	
	redata = GSM_RX(len);   //接收数据

	if(len == 0)
	{
			return GSM_FALSE;
	}
	//本程序兼容GSM900A、GSM800A、GSM800C
	if (strstr(redata,"SIMCOM_GSM900A") != 0)
	{
		return GSM_TRUE;
	}
	else if(strstr(redata,"SIMCOM_SIM800") != 0)
	{
		return GSM_TRUE;
	}
	else
		return GSM_FALSE;

}

/*---------------------------------------------------------------------*/

//发起拨打电话（不管接不接通）
void gsm_call(char *num)
{
		GSM_DEBUG_FUNC();  

    GSM_CLEAN_RX();                 //清空了接收缓冲区数据
    
		GSM_DEBUG("GSM printf:ATD%s;\r",num);

    GSM_USART_printf("ATD%s;\r",num);

    //拨打后是不返回数据的
    //不管任何应答，开头都是"\r\n",即真正有效的数据是从第3个字节开始
    //对方挂掉电话（没接通），返回：BUSY
    //对方接听了电话：+COLP: "555",129,"",0,"9648674F98DE"   OK
    //对方接听了电话,然后挂掉：NO CARRIER
    //对方超时没接电话：NO ANSWER
}

//来电电话
//0表示成功，1表示失败
uint8_t IsRing(char *num)
{
    char *redata;
		char * redata_off;
    uint8_t len;
    		
	  GSM_DEBUG_FUNC();  

    if(gsm_cmd_check("RING"))
    {
        return GSM_FALSE;
    }

    
    redata = GSM_RX(len);   //接收数据
		

//不使用CLIP使能来电显示的话，响应只有：
//		RING
//
		
//使用CLIP使能来电显示后，响应为：
		
//		RING
//
//+CLIP: "电话号码",161,"",0,"",0
		
		//偏移至响应的字符串地址
		redata_off = strstr(redata,"RING");
		
		if(redata_off == NULL)
			return GSM_FALSE;
		
		//计算偏移后剩余的长度
		len = len - (redata_off - redata); 
		
    if(len == 0)
    {
        return GSM_FALSE;
    }
    
    //第一个冒号后面的数据为:”号码“
    while(*redata != ':')
    {
        len--;
        if(len==0)
        {
            return GSM_FALSE;
        }
        redata++;
    }
    redata+=3;
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //字符串结尾需要清0
	GSM_CLEAN_RX();
    return GSM_TRUE;
}



//检测是否有卡
//0表示成功，1表示失败
uint8_t IsInsertCard(void)
{
	GSM_DEBUG_FUNC();
	
	GSM_CLEAN_RX();
	return gsm_cmd("AT+CNUM\r","OK",200);
   
}

//检测是否有信号
uint8_t IsNOCARRIER(void)
{
  if(gsm_cmd_check("NO CARRIER"))
  {
    return GSM_FALSE;
  }  
  return GSM_TRUE;
}

/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/**
 * @brief  IsASSIC 判断是否纯ASSIC编码
 * @param  str: 字符串指针
 * @retval 纯ASSIC编码返回TRUE，非纯ASSIC编码返回FALSE
 */
uint8_t IsASSIC(char * str)
{
		GSM_DEBUG_FUNC();  

    while(*str)
    {
        if(*str>0x7F)
        {
            return GSM_FALSE;
        }
        str++;
    }
    
    return GSM_TRUE;
}


/**
 * @brief  gsm_gbk2ucs2 把GBK编码转换成UCS2编码
 * @param  ucs2: ucs2字符串指针，gbk：GBK字符串指针
 * @retval 无
 */
void gsm_gbk2ucs2(char * ucs2,char * gbk)
{
    WCHAR   tmp;
	
		GSM_DEBUG_FUNC();  

    
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //英文
        {
            
            *ucs2++ = 0;
            *ucs2++ = *gbk++;  
        }
        else                        //中文
        {
            tmp = GSM_SWAP16(*(WCHAR *)gbk);
            
            *(WCHAR *)ucs2 = GSM_SWAP16(ff_convert(tmp,1));
            gbk+=2;
            ucs2+=2;
        }
    }
}

/**
 * @brief  gsm_char2hex 把字符转换成16进制字符
 * @param  hex: 16进制字符存储的位置指针，ch：字符
 * @retval 无
 */
void gsm_char2hex(char *hex,char ch)
{
    const char numhex[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
   
		GSM_DEBUG_FUNC();  

		*hex++  = numhex[(ch & 0xF0)>>4];
    *hex    = numhex[ ch & 0x0F];
}


/**
 * @brief  gsm_gbk2ucs2hex GBK编码转ucs2编码再转成16进制字符
 * @param  ucs2hex: 16进制数存储的位置指针，gbk：字符
 * @retval 无
 */
void gsm_gbk2ucs2hex(char * ucs2hex,char * gbk)
{
    WCHAR   tmp;
    
	  GSM_DEBUG_FUNC();  
	
    while(*gbk)
    {
        if((*gbk&0xFF) < 0x7F)      //英文
        {
            
            *ucs2hex++ = '0';
            *ucs2hex++ = '0';
            gsm_char2hex(ucs2hex,*gbk);
            ucs2hex+=2;
            gbk++;  
        }
        else                        //中文
        {
            tmp = GSM_SWAP16(*(WCHAR *)gbk);
            tmp = ff_convert(tmp,1);
            gsm_char2hex(ucs2hex,(char)(tmp>>8));
            ucs2hex+=2;
            gsm_char2hex(ucs2hex,(char)tmp);
            ucs2hex+=2;
            gbk+=2;
        }
    }
    *ucs2hex=0;
}



//发送短信（支持中英文,中文为GBK码）
/**
 * @brief  gsm_sms 发送短信（支持中英文,中文为GBK码）
 * @param  num: 电话号码，smstext：短信内容
 * @retval 无
 */
uint8_t gsm_sms(char *num,char *smstext)
{
    char converbuff[160];
	  char cmdbuff[80];
	  char end= 0x1A;
	  uint8_t testSend = 0;
		
	  GSM_DEBUG_FUNC(); 

    GSM_CLEAN_RX();                 //清空了接收缓冲区数据
	
    if(IsASSIC(smstext)==GSM_TRUE)
    {
			//英文
			//"GSM"字符集
			if(gsm_cmd("AT+CSCS=\"GSM\"\r","OK", 100) != GSM_TRUE) goto sms_failure;

			//文本模式
			if(gsm_cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE) goto sms_failure;

			//生成电话号码命令 
			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",num);
			//复制短信内容
			strcpy(converbuff,smstext);

    }
    else
    {
			//中文
			//"UCS2"字符集
			if(gsm_cmd("AT+CSCS=\"UCS2\"\r","OK", 100) != GSM_TRUE) goto sms_failure;
			//文本模式
			if(gsm_cmd("AT+CMGF=1\r","OK", 100) != GSM_TRUE) goto sms_failure;

			if(gsm_cmd("AT+CSMP=17,167,0,8\r","OK", 100) != GSM_TRUE) goto sms_failure;

			//生成电话号码命令，UCS2的电话号码(需要转成 ucs2码)
			gsm_gbk2ucs2hex(converbuff,num);//转换为UCS2
			sprintf(cmdbuff,"AT+CMGS=\"%s\"\r",converbuff);  
			
			//转换短信内容
			gsm_gbk2ucs2hex(converbuff,smstext);
    }
		
	//发送电话号码	
	if(gsm_cmd(cmdbuff,">",200) == 0)
	{
		GSM_USART_printf("%s",converbuff);//发送短信内容
		
		GSM_DEBUG("Send String:%s",converbuff);

		GSM_CLEAN_RX();
		gsm_cmd(&end,0,100);		//0x1A结束符
		
		//检测是否发送完成
		while(gsm_cmd_check("OK") != GSM_TRUE )
		{		
			if(++testSend >100)//最长等待10秒
			{
				goto sms_failure;
			}
			GSM_DELAY(100); 		
		}	
		return GSM_TRUE;
	}
	else
	{				
sms_failure:		
		end = 0x1B;
		gsm_cmd(&end,0,0);	//ESC,取消发送 
		return GSM_FALSE;		
	}		
}




//查询是否接收到新短信
//0:无新短信，非0：新短信地址
uint8_t IsReceiveMS(void)
{
	char address[3]={0};
	uint8_t addressnum=0;
	char *redata;
	char *redata_off;
    uint8_t len;
	
			GSM_DEBUG_FUNC();  

/*------------- 查询是否有新短信并提取存储位置 ----------------------------*/
    if(gsm_cmd_check("+CMTI:"))
    {
        return 0;
    }
   
    redata = GSM_RX(len);   //接收数据
		
		//偏移至响应的字符串地址
		redata_off = strstr(redata,"+CMTI: \"SM\",");
		
		if(redata_off == NULL)
			return 0;
		
		//计算偏移后剩余的长度
		len = len - (redata_off - redata); 
		
//		printf("CMTI:redata:%s,len=%d\n",redata,len);
    if(len == 0)
    {
        return 0;
    }
    
    //第一个逗号后面的数据为:”短信存储地址“
    while(*redata_off != ',')
    {
        len--;
        if(len==0)
        {
            return 0;
        }
        redata_off++;
    }
	redata_off+=1;//去掉；','
	address[0]= *redata_off++;
	address[1]= *redata_off++;
	address[2]= *redata_off++;
	if((address[2]>='0')&&(address[2]<='9'))
		addressnum=(address[0]-'0')*100+(address[1]-'0')*10+(address[2]-'0');
	else
		addressnum=(address[0]-'0')*10+(address[1]-'0');
	
//	printf("address:%c%c%c->%d\n",address[0],address[1],address[2],addressnum);	
    return addressnum;
}	

//读取短信内容
//形参：	messadd：	短信地址
//			num：		保存发件人号码(unicode编码格式的字符串)
//			str：		保存短信内容(unicode编码格式的字符串)
//返回值：	0表示失败
//			1表示成功读取到短信，该短信未读（此处是第一次读，读完后会自动被标准为已读）
//			2表示成功读取到短信，该短信已读
uint8_t readmessage(uint8_t messadd,char *num,char *str)
{
	char *redata,cmd[20]={0};
    uint8_t len;
	char result=0;
	
	GSM_DEBUG_FUNC();  

	
	GSM_CLEAN_RX();                 //清空了接收缓冲区数据
	if(messadd>MaxMessAdd)return 0;
	
/*------------- 读取短信内容 ----------------------------*/
	sprintf(cmd,"AT+CMGR=%d\r",messadd);	
		  
	if(gsm_cmd(cmd,"+CMGR:",500) != GSM_TRUE)
	{
		printf("GSM_FALSE");
		return 0;
	}
	
	redata = GSM_RX(len);   //接收数据

	if(len == 0)
	{
		return 0;
	}
//	printf("CMGR:redata:%s\nlen=%d\n",redata,len);

	if(strstr(redata,"UNREAD")==0)result=2;
	else	result=1;
	//第一个逗号后面的数据为:”发件人号码“
	while(*redata != ',')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
		
	}
	redata+=2;//去掉',"'
	while(*redata != '"')
	{
		*num++ = *redata++;
		len--;
	}
	*num = 0;               //字符串结尾需要清0
	
	while(*redata != '+')
	{
		len--;
		if(len==0)
		{
			return 0;
		}
		redata++;
	}
	redata+=6;//去掉'+32"\r'
	while(*redata != '\r')
	{
		*str++ = *redata++;
	}
	*str = 0;               //字符串结尾需要清0
	
//	printf("CMGR:result:%d\n",result);
	return result;
}

#include "ff.h"//firecc936.c文件中：ff_convert()函数实现unicode与gbk互转功能

uint8_t hexuni2gbk(char *hexuni,char *chgbk)
{
	uint8_t len=0,i=0;
	WCHAR wgbk=0;
	WCHAR tmp=0;
	uint8_t unitmp[4]={0};
				
	GSM_DEBUG_FUNC();  
	
	GSM_CLEAN_RX();
	len=strlen(hexuni);
	if(!len)return GSM_FALSE;
	//printf("hexuni:%s::len:%d\n",hexuni,len);
	for(i=0;i<len/4;++i)
	{
		if(hexuni[4*i]>=0x41)	unitmp[0]=hexuni[4*i]-0x41+10;
		else	unitmp[0]=hexuni[4*i]-0x30;
		if(hexuni[4*i+1]>=0x41)unitmp[1]=hexuni[4*i+1]-0x41+10;
		else	unitmp[1]=hexuni[4*i+1]-0x30;
		if(hexuni[4*i+2]>=0x41)unitmp[2]=hexuni[4*i+2]-0x41+10;
		else	unitmp[2]=hexuni[4*i+2]-0x30;
		if(hexuni[4*i+3]>=0x41)unitmp[3]=hexuni[4*i+3]-0x41+10;
		else	unitmp[3]=hexuni[4*i+3]-0x30;
		
		tmp=unitmp[0]*0x1000+unitmp[1]*0x100+unitmp[2]*16+unitmp[3];
		wgbk=ff_convert(tmp,0);
		//printf("tmp:%X->wgbk:%X\n",tmp,wgbk);
		
		if(wgbk<0x80)
		{
			*chgbk=(char)wgbk;
			chgbk++;
		}
		else
		{
			*chgbk=(char)(wgbk>>8);
			chgbk++;
			*chgbk=(char)wgbk;
			chgbk++;
		}		
	}	
	*chgbk=0;
	return GSM_TRUE;		
}




/*---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
//void gsm_gprs_init(void)
//{
//    GSM_USART_printf("AT+CGCLASS=\"B\"\r");                       //设置移动台类别为"B"
//    GSM_DELAY(100);
//    GSM_USART_printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r");          //PDP上下文标识1，互联网协议，接入点名称：CMNET
//    GSM_DELAY(100);
//    GSM_USART_printf("AT+CGATT=1\r");                             //附着 GPRS 网络
//    GSM_DELAY(100);
//    GSM_USART_printf("AT+CIPCSGP=1,\"CMNET\"\r");                 //设置为 GPRS 连接模式 ，接入点为CMNET
//}

//void gsm_gprs_tcp_link(char *localport,char * serverip,char * serverport)
//{
//    GSM_USART_printf("AT+CLPORT=\"TCP\",\"%s\"\r",localport);         //获取本地端口
//    GSM_DELAY(100);

//    //设置服务器IP和端口
//    GSM_USART_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
//}

//void gsm_gprs_udp_link(char *localport,char * serverip,char * serverport)
//{
//    GSM_USART_printf("AT+CLPORT=\"UDP\",\"%s\"\r",localport);              //获取本地端口
//    GSM_DELAY(100);

//    GSM_USART_printf("AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);   //设置服务器IP和端口
//}

//void gsm_gprs_send(char * str)
//{
//    GSM_USART_printf("AT+CIPSEND\r");
//    GSM_DELAY(100);
//        
//    GSM_USART_printf("%s%c",str,0x1A);
//}

//void gsm_gprs_link_close(void)               //IP链接断开
//{
//    GSM_USART_printf("AT+CIPCLOSE=1\r");
//}

/**
 * @brief  初始化GPRS网络
 * @param  无
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_gprs_init(void)
{
	GSM_DEBUG_FUNC();  
		
	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CGCLASS=\"B\"\r","OK", 200) != GSM_TRUE) return GSM_FALSE;

	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r","OK", 200) != GSM_TRUE) return GSM_FALSE;		

	GSM_CLEAN_RX();	
	if(gsm_cmd("AT+CGATT=1\r","OK", 200)!= GSM_TRUE) return GSM_FALSE;

	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CIPCSGP=1,\"CMNET\"\r","OK", 200)!= GSM_TRUE) return GSM_FALSE;
	
	return GSM_TRUE;
}

/**
 * @brief  建立TCP连接，最长等待时间20秒，可自行根据需求修改
	* @param  localport: 本地端口
	* @param  serverip: 服务器IP
	* @param  serverport: 服务器端口
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_gprs_tcp_link(char *localport,char * serverip,char * serverport)
{
	char cmd_buf[100];
	uint8_t testConnect=0;
	
	GSM_DEBUG_FUNC();  
		
	sprintf(cmd_buf,"AT+CLPORT=\"TCP\",\"%s\"\r",localport);
	
	if(gsm_cmd(cmd_buf,"OK", 100)!= GSM_TRUE)
		return GSM_FALSE;
	
	GSM_CLEAN_RX();
		
	sprintf(cmd_buf,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);	
	gsm_cmd(cmd_buf,0, 100);	
	
	//检测是否建立连接
	while(gsm_cmd_check("CONNECT OK") != GSM_TRUE)
	{		
		if(++testConnect >200)//最长等待20秒
		{
			return GSM_FALSE;
		}
		GSM_DELAY(100); 		
	}				
	return GSM_TRUE;
}

/**
 * @brief  建立UDP连接，最长等待时间20秒，可自行根据需求修改
	* @param  localport: 本地端口
	* @param  serverip: 服务器IP
	* @param  serverport: 服务器端口
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_gprs_udp_link(char *localport,char * serverip,char * serverport)
{
	char cmd_buf[100];
	uint8_t testConnect=0;
	
	GSM_DEBUG_FUNC();  

	sprintf(cmd_buf,"AT+CLPORT=\"UDP\",\"%s\"\r",localport);
	
	if(gsm_cmd(cmd_buf,"OK", 100)!= GSM_TRUE)
		return GSM_FALSE;
	
	GSM_CLEAN_RX();
		
	sprintf(cmd_buf,"AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r",serverip,serverport);
	
	gsm_cmd(cmd_buf,0, 100);	
	
		//检测是否建立连接
	while(gsm_cmd_check("CONNECT OK") != GSM_TRUE )
	{
		
		if(++testConnect >200)//最长等待20秒
		{
			return GSM_FALSE;
		}
		GSM_DELAY(100); 		
	}		
		
	return GSM_TRUE;
}

/**
 * @brief  使用GPRS发送数据，发送前需要先建立UDP或TCP连接
	* @param  str: 要发送的数据
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_gprs_send(const char * str)
{
	char end = 0x1A;
	uint8_t testSend=0;
	
	GSM_DEBUG_FUNC();

	GSM_CLEAN_RX();
	
	if(gsm_cmd("AT+CIPSEND\r",">",500) == 0)
	{
		GSM_USART_printf("%s",str);
		GSM_DEBUG("Send String:%s",str);

		GSM_CLEAN_RX();
		gsm_cmd(&end,0,100);		
		
		//检测是否发送完成
		while(gsm_cmd_check("SEND OK") != GSM_TRUE )
		{		
			if(++testSend >200)//最长等待20秒
			{
				goto gprs_send_failure;
			}
			GSM_DELAY(100); 		
		}	
		return GSM_TRUE;
	}
	else
	{
		
gprs_send_failure:
		
		end = 0x1B;
		gsm_cmd(&end,0,0);	//ESC,取消发送 

		return GSM_FALSE;
	}
}

/**
 * @brief  断开网络连接
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_gprs_link_close(void)              //IP链接断开
{
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();
	if(gsm_cmd("AT+CIPCLOSE=1\r","OK",200) != GSM_TRUE)
    {
        return GSM_FALSE;
    }
	return GSM_TRUE;
}

/**
 * @brief  关闭场景
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t gsm_gprs_shut_close(void)               //关闭场景
{
	uint8_t  check_time=0;
	GSM_DEBUG_FUNC();  

	GSM_CLEAN_RX();	
	gsm_cmd("AT+CIPSHUT\r",0,0) ;
	while(	gsm_cmd_check("OK") != GSM_TRUE)
	{
		if(++check_time >50)
			return GSM_FALSE;
		
		GSM_DELAY(200);
	}

	return GSM_TRUE;
}

/**
 * @brief  返回从GSM模块接收到的网络数据，打印到串口
 * @retval 失败GSM_FALSE  成功GSM_TRUE
 */
uint8_t PostGPRS(void)
{

	char *redata;
	uint8_t len;

	GSM_DEBUG_FUNC();  

	redata = GSM_RX(len);   //接收数据 

	if(len == 0)
	{
			return GSM_FALSE;
	}
	
	printf("PostTCP:%s\n",redata);
	GSM_CLEAN_RX();
	
	return GSM_TRUE;

}



/*---------------------------------------------------------------------*/
