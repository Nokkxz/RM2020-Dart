#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "M8266WIFI_set.h"
#include "main.h"

u16 WIFI_Status = 0;
u8  link_no=0;
void WIFI_Set(void)
{
    M8266HostIf_Init();
    M8266WIFI_Module_Init_Via_SPI();
    
    //  u8 M8266WIFI_SPI_Setup_Connection(u8 tcp_udp, u16 local_port, char remote_addr, u16 remote_port, u8 link_no, u8 timeout_in_s, u16* status);
	if(M8266WIFI_SPI_Setup_Connection(WIFI_CONNECTION_TYPE, WIFI_LOCAL_PORT, WIFI_REMOTE_ADDR, WIFI_REMOTE_PORT, link_no, 20, &WIFI_Status)==0)
	{
		 while(1){}
	}
}


int WIFI_len=1000;
int WIFI_time=20;
u8 *ov_buff_p = &ov_buff[0];
u8 WIFI_SOF[]={0xff,0xff};
u8 WIFI_info[10]={1,1,1,1,1,1,1,1,1,1};

void WIFI_Send(void)
{
    switch (WIFI_MODE)
    {
        case 1:
            WIFI_Send_Data((u8*)&WIFI_SOF,1);
            for(u32 i=0;i<WIFI_time;i++)
            {
                WIFI_Status=0;
                WIFI_Send_Data(ov_buff_p+(i*WIFI_len),WIFI_len);
            }
            WIFI_Send_Data((u8*)&WIFI_SOF,2);
            break;
        case 2:
            WIFI_info[0] = info.x_MAX;
            WIFI_info[1] = info.y_MAX;
            WIFI_info[2] = (u8)(info.gray_MAX>>8);
            WIFI_info[3] = (u8)(info.gray_MAX);
            WIFI_info[4] = (u8)(info.r_MAX>>8);
            WIFI_info[5] = (u8)(info.r_MAX);
            WIFI_info[6] = (u8)(info.g_MAX>>8);
            WIFI_info[7] = (u8)(info.g_MAX);
            WIFI_info[8] = (u8)(info.b_MAX>>8);
            WIFI_info[9] = (u8)(info.b_MAX);
            WIFI_Send_Data((u8*)&WIFI_info,10);
            break;
        default:
            break;
    }
}

