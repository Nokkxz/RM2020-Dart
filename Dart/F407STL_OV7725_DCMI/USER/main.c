#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "timer.h"
#include "led.h"
#include "ov7725.h"
#include "dcmi.h"
#include "M8266HostIf.h"
#include "M8266WIFIDrv.h"
#include "M8266WIFI_ops.h"
#include "M8266WIFI_set.h"
#include "brd_cfg.h"
#include "main.h"

u8 ov_buff[OV_BUFF_SIZE];

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
//	uart_init(115200);
	TIM3_Int_Init(10000-1,8400-1);
    
	while(OV7725_Init())
		delay_ms(500);
    OV7725_Light_Mode(1);
	OV7725_Color_Saturation(-4);
	OV7725_Brightness(-4);
	OV7725_Contrast(-4);
	OV7725_Special_Effects(1);
    OV7725_Window_Set(OV_WIDTH,OV_HEIGHT,0);
    
    if(WIFI_MODE != 0) WIFI_Set();
    
	LED_Init();
	My_DCMI_Init();
	DCMI_DMA_Init((u32)&ov_buff,DMA_BUFF_SIZE,DMA_MemoryDataSize_Byte);
	DCMI_Start();
    
    while(1){}
}
