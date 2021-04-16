#ifndef _OV7725_H
#define _OV7725_H
#include "sys.h"
#include "sccb.h"


//#define OV7725_Periph       RCC_AHB1Periph_GPIOE
//#define OV7725_GPIO         GPIOE
//#define OV7725_PWDN_Pin     GPIO_Pin_11
//#define OV7725_RST_Pin      GPIO_Pin_13
//#define OV7725_PWDN_Num     11
//#define OV7725_RST_Num      13

//#define OV7725_PWDN  	PEout(OV7725_PWDN_Num)
//#define OV7725_RST  	PEout(OV7725_RST_Num)

/////////////////////////////////////////									

#define OV7725_MID				0X7FA2
#define OV7725_PID				0X7721

    
/////////////////////////////////////////									
	    				 
u8   OV7725_Init(void);		  	   		 
void OV7725_Light_Mode(u8 mode);
void OV7725_Color_Saturation(s8 sat);
void OV7725_Brightness(s8 bright);
void OV7725_Contrast(s8 contrast);
void OV7725_Special_Effects(u8 eft);
void OV7725_Window_Set(u16 width,u16 height,u8 mode);

#endif
