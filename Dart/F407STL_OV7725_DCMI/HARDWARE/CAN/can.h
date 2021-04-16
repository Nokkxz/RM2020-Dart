#ifndef _CAN_H
#define _CAN_H

#include "main.h"

void CAN2_Init(void);
void CAN2_RX0_IRQHandler(void);
u8 CAN2_Send_Msg(u8* msg,u8 len,u8 stdid);












#endif
