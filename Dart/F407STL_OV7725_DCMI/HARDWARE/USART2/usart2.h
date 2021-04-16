#ifndef __USART2_H
#define __USART2_H 
#include "sys.h"
#include "stdio.h"	  

//Send EndOfFrame
#define USART2_SendEOF()\
            USART_SendData(USART2,0xFFFF);\
            while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET){;}

#define USART2_SendData(data)\
            USART_SendData(USART2,data);\
            while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET){;}

void usart2_init(u32 pclk1,u32 bound);
#endif	   
















