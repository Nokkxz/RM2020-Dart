#ifndef _M8266WIFI_SET_
#define _M8266WIFI_SET_

#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"

//0:UDP  2:TCP
#define WIFI_CONNECTION_TYPE    2
#define WIFI_LOCAL_PORT         8000
#define WIFI_REMOTE_ADDR        "192.168.4.2"
#define WIFI_REMOTE_PORT        8001

#define WIFI_Send_Data(data,size)   M8266WIFI_SPI_Send_Data(data, (u32)size, 0, &WIFI_Status);

extern u16 WIFI_Status;
void WIFI_Set(void);
void WIFI_Send(void);








#endif
