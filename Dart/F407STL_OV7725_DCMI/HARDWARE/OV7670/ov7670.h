#ifndef _OV7725_H
#define _OV7725_H
#include "sys.h"
#include "sccb.h"


//#define OV7670_Periph       RCC_AHB1Periph_GPIOE
//#define OV7670_GPIO         GPIOE
//#define OV7670_PWDN_Pin     GPIO_Pin_11
//#define OV7670_RST_Pin      GPIO_Pin_13
//#define OV7670_PWDN_Num     11
//#define OV7670_RST_Num      13

//#define OV7670_PWDN  	PEout(OV7670_PWDN_Num)
//#define OV7670_RST  	PEout(OV7670_RST_Num)

/////////////////////////////////////////									

#define OV7670_Set() \
	while(OV7670_Init())\
	{\
		delay_ms(500);\
	}\
    OV7670_Light_Mode(0);\
	OV7670_Color_Saturation(0);\
	OV7670_Brightness(0);\
	OV7670_Contrast(0);\
	OV7670_Special_Effects(0);\

u8   OV7670_Init(void);
void OV7670_Light_Mode(u8 mode);
void OV7670_Color_Saturation(s8 sat);
void OV7670_Brightness(s8 bright);
void OV7670_Contrast(s8 contrast);
void OV7670_Special_Effects(u8 eft);
void OV7670_Window_Set(u16 sx,u16 sy,u16 width,u16 height);

#endif
