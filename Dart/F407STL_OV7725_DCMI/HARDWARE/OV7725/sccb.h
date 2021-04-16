#ifndef __SCCB_H
#define __SCCB_H
#include "sys.h"

//引脚设置 //TBD
#define SCCB_Periph     RCC_AHB1Periph_GPIOB
#define SCCB_GPIO       GPIOB
#define SCCB_SCL_Pin    GPIO_Pin_8
#define SCCB_SDA_Pin    GPIO_Pin_9
#define SCCB_SCL_Num    8
#define SCCB_SDA_Num    9

//IO操作函数 //TBD
#define SCCB_SCL    		PBout(SCCB_SCL_Num)
#define SCCB_SDA    		PBout(SCCB_SDA_Num)
#define SCCB_READ_SDA    	PBin(SCCB_SDA_Num)

//IO方向设置
#define SCCB_SDA_IN()  {SCCB_GPIO->MODER&=~(3<<(SCCB_SDA_Num*2));SCCB_GPIO->MODER|=0<<SCCB_SDA_Num*2;}
#define SCCB_SDA_OUT() {SCCB_GPIO->MODER&=~(3<<(SCCB_SDA_Num*2));SCCB_GPIO->MODER|=1<<SCCB_SDA_Num*2;}

#define SCCB_ID   			0X42  			//OV7670的ID

///////////////////////////////////////////
void SCCB_Init(void);
void SCCB_Start(void);
void SCCB_Stop(void);
void SCCB_No_Ack(void);
u8 SCCB_WR_Byte(u8 dat);
u8 SCCB_RD_Byte(void);
u8 SCCB_WR_Reg(u8 reg,u8 data);
u8 SCCB_RD_Reg(u8 reg);
#endif
