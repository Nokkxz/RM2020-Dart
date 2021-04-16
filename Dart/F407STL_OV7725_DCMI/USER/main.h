#ifndef _MAIN_H
#define _MAIN_H

#include "sys.h"

//u16
#define OV_HEIGHT   100
#define OV_WIDTH    100

//数据缓存(u8)
#define OV_BUFF_SIZE        OV_HEIGHT*OV_WIDTH*2    //<120k

//数据传输量(u32)
#define DMA_BUFF_SIZE       OV_BUFF_SIZE/4

#define WIFI_MODE   2

typedef struct
{
    u8 x_MAX,y_MAX;
    u16 gray_MAX,r_MAX,g_MAX,b_MAX;
}Info;
extern Info info;

extern u8 ov_buff[OV_BUFF_SIZE];

#endif
