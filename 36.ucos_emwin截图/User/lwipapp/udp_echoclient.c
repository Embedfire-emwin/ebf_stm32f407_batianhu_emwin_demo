/**
  ******************************************************************************
  * @file    udp_echoclient.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013
  * @brief   UDP echo client
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "netconf.h"
#include "./LAN8742A/LAN8742A.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "udp_echoclient.h"
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

static uint8_t   data[100];
static __IO uint32_t message_count = 0;

struct udp_pcb *upcb;
extern DRV_NETWORK drv_network;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Connect to UDP echo server
  * @param  None
  * @retval None
  */
uint8_t udp_echoclient_connect(DRV_NETWORK network)
{

  struct pbuf *p;
  struct ip_addr DestIPaddr;
  err_t err;
       
  /* Create a new UDP control block  */
  upcb = udp_new();
  
  if (upcb!=NULL)
  {
    /*assign destination IP address */
    IP4_ADDR( &DestIPaddr, network.net_remote_ip1, network.net_remote_ip2, network.net_remote_ip3, network.net_remote_ip4 );
  
		#ifdef SERIAL_DEBUG
		printf("\nDestIPaddr: %d.%d.%d.%d\n",network.net_remote_ip1, network.net_remote_ip2, network.net_remote_ip3, network.net_remote_ip4);
    #endif
		
    /* configure destination IP address and port */
    err= udp_connect(upcb, &DestIPaddr, network.net_remote_port);
    
    if (err == ERR_OK)
    {
      /* Set a receive callback for the upcb */
      udp_recv(upcb, udp_receive_callback, NULL);
      
      sprintf((char*)data, "sending udp client message %d\n",message_count);
  
      /* allocate pbuf from pool*/
      p = pbuf_alloc(PBUF_TRANSPORT,strlen((char*)data), PBUF_POOL);
      
      if (p != NULL)
      {
        /* copy data to pbuf */
        pbuf_take(p, (char*)data, strlen((char*)data));
          
        /* send udp data */
        udp_send(upcb, p); 
        
        /* free pbuf */
        pbuf_free(p);
        return 0;
      }
      else
      {
        /* free the UDP connection, so we can accept new clients */
        udp_remove(upcb);
        #ifdef SERIAL_DEBUG
        printf("\n\r can not allocate pbuf ");
        #endif
        return 1;
      }
    }
    else
    {
      /* free the UDP connection, so we can accept new clients */
      udp_remove(upcb);
      #ifdef SERIAL_DEBUG
       printf("\n\r can not connect udp pcb");
      #endif
      return 1;
    }
  }
  else
  {
    #ifdef SERIAL_DEBUG
     printf("\n\r can not create udp pcb");
    #endif
    return 1;
  } 
}
uint8_t udp_echoclient_send(char *data)
{
  struct pbuf *p;
  err_t err;
  /* allocate pbuf from pool*/
  p = pbuf_alloc(PBUF_TRANSPORT,strlen(data), PBUF_POOL);
  
  if (p != NULL)
  {
    /* copy data to pbuf */
    err = pbuf_take(p,data, strlen(data));
    if(err==ERR_OK) 
    {
      /* send udp data */
      udp_send(upcb, p); 
      
      /* free pbuf */
      pbuf_free(p);
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 1;
  }
}

/**
  * @brief This function is called when an UDP datagrm has been received on the port UDP_PORT.
  * @param arg user supplied argument (udp_pcb.recv_arg)
  * @param pcb the udp_pcb which received data
  * @param p the packet buffer that was received
  * @param addr the remote IP address from which the packet was received
  * @param port the remote port from which the packet was received
  * @retval None
  */
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
	char *recdata=0;
	/*increment message count */
  message_count++;
	
	if(p !=NULL)
	{		
    recdata=(char *)malloc(p->len*sizeof(char)*2);		
		if(recdata!=NULL)
		{
      com_data2null((uint8_t *)recdata,p->len*sizeof(char)*2);
			memcpy(recdata,p->payload,p->len);
      #ifdef SERIAL_DEBUG
			printf("upd_rec:%s",recdata); 
      #endif
      com_gbk2utf8(recdata,recdata);
      MULTIEDIT_AddText(WM_GetDialogItem(drv_network.hWin, GUI_ID_MULTIEDIT1), recdata);
		}
		free(recdata);
    
		udp_send(upcb,p);
		/* Free receive pbuf */
		pbuf_free(p);
	}
  
//  /* free the UDP connection, so we can accept new clients */
//  udp_remove(upcb);   
}

/**
  * @brief  Disconnect to UDP echo server
  * @param  None
  * @retval None
  */
void udp_echoclient_disconnect(void)
{
	#ifdef SERIAL_DEBUG
	printf("Disconnect to UDP echo server");
	#endif
	 /* free the UDP connection, so we can accept new clients */
  udp_remove(upcb);
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
