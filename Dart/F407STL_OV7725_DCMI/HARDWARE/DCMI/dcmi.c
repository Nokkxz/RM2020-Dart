#include "sys.h"
#include "dcmi.h"
#include "ov2640.h"
#include "main.h"
#include "usart2.h"
#include "M8266WIFI_set.h"
#include "delay.h"

#define K_R 1
#define K_G 7
#define K_B 3

u16 ov_frame=0;
u16 ov_line=0;
u16 dma_times=0;

u8 r,g,b;
u16 gray;

Info info;

void DMA2_Stream1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1)==SET)//行传输完成
    {
        DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
        
        dma_times++;
    }
}


void DCMI_IRQHandler(void)
{
    if(DCMI_GetITStatus(DCMI_IT_FRAME)==SET)    //帧中断
    {
        DCMI_ClearITPendingBit(DCMI_IT_FRAME);
        
        //RGB calculation
        for(u32 i=0; i<OV_BUFF_SIZE-2; i+=2)
        {
            r=ov_buff[i]&0xf8;
            g=(ov_buff[i]<<5) | (ov_buff[i+1]&0xe0>>3);
            b=ov_buff[i+1]<<3&0xff;
            
            gray = r + (g<<2) + (b<<1);
            
            if(gray>info.gray_MAX)
            {
                info.r_MAX=r;
                info.g_MAX=g;
                info.b_MAX=b;
                info.gray_MAX=gray;
                info.x_MAX=i%OV_WIDTH;
                info.y_MAX=(i/OV_WIDTH)>>1;
            }
        }
        
        WIFI_Send();
        
        info.gray_MAX=0;
        ov_frame++;
        ov_line=0;
        
    }
    else if(DCMI_GetITStatus(DCMI_IT_LINE)==SET)
    {
        DCMI_ClearITPendingBit(DCMI_IT_LINE);

        ov_line++;
    }
}

//DCMI DMA配置
//DMA_Memory0BaseAddr:存储器地址
//DMA_BufferSize:存储器长度    0~65535
//DMA_MemoryDataSize:存储器位宽
//DMA_MemoryDataSize:存储器位宽
//DMA_MemoryInc:存储器增长方式
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize)
{ 
	DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置
	
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;  //通道1 DCMI通道
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DCMI->DR;//外设地址为:DCMI->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = DMA_Memory0BaseAddr;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = DMA_BufferSize;//数据传输量
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//外设数据长度:32位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize;//存储器数据长度
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; //FIFO模式        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//使用全FIFO 
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);//初始化DMA Stream
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);//开启传输完成中断
}

void My_DCMI_Init(void)
{
    DCMI_InitTypeDef DCMI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI,ENABLE);//使能DCMI时钟
    
    #ifdef F407IG
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);//使能GPIOA/C/E/I时钟
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能输出
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PA6
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;// PC7
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;//PE0/1/4/5/6
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;//PH8/9
    GPIO_Init(GPIOH, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;//PI4/5
    GPIO_Init(GPIOI, &GPIO_InitStructure);
    
	GPIO_PinAFConfig(GPIOH,GPIO_PinSource8,GPIO_AF_DCMI); //PA4,AF13  DCMI_HSYNC    //PH8
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_DCMI); //PA6,AF13  DCMI_PCLK     //PA6
 	GPIO_PinAFConfig(GPIOI,GPIO_PinSource5,GPIO_AF_DCMI); //PB7,AF13  DCMI_VSYNC    //PI5
 	GPIO_PinAFConfig(GPIOH,GPIO_PinSource9,GPIO_AF_DCMI); //PC6,AF13  DCMI_D0       //PH9
 	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_DCMI); //PC7,AF13  DCMI_D1       //PC7
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource0,GPIO_AF_DCMI); //PC8,AF13  DCMI_D2       //PE0
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource1,GPIO_AF_DCMI); //PC9,AF13  DCMI_D3       //PE1
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource4,GPIO_AF_DCMI); //PC11,AF13 DCMI_D4       //PE4
	GPIO_PinAFConfig(GPIOI,GPIO_PinSource4,GPIO_AF_DCMI); //PB6,AF13  DCMI_D5       //PI4
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_DCMI); //PE5,AF13  DCMI_D6       //PE5
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_DCMI); //PE6,AF13  DCMI_D7       //PE6

    #endif
	
	DCMI_DeInit();//清除原来的设置 
    
    DCMI_InitStructure.DCMI_CaptureMode=DCMI_CaptureMode_Continuous;//连续模式
	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_All_Frame;//全帧捕获
	DCMI_InitStructure.DCMI_ExtendedDataMode= DCMI_ExtendedDataMode_8b;//8位数据格式
	DCMI_InitStructure.DCMI_SynchroMode= DCMI_SynchroMode_Hardware;//硬件同步HSYNC,VSYNC
	DCMI_InitStructure.DCMI_VSPolarity=DCMI_VSPolarity_High;//VSYNC 高电平有效
	DCMI_InitStructure.DCMI_HSPolarity= DCMI_HSPolarity_Low;//HSYNC 低电平有效//Low
	DCMI_InitStructure.DCMI_PCKPolarity= DCMI_PCKPolarity_Rising;//PCLK 上升沿有效//Rising
	DCMI_Init(&DCMI_InitStructure);
    
    //DCMI_IT_LINE|DCMI_IT_FRAME
	DCMI_ITConfig(DCMI_IT_LINE|DCMI_IT_FRAME,ENABLE);
	
	DCMI_Cmd(ENABLE);	//DCMI使能
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

}

void DCMI_Start(void)
{
	DMA_Cmd(DMA2_Stream1, ENABLE);//开启DMA2,Stream1
	DCMI_CaptureCmd(ENABLE);//DCMI捕获使能
}

void DCMI_Stop(void)
{
    DCMI_CaptureCmd(DISABLE);//DCMI捕获使关闭
	while(DCMI->CR&0X01);		//等待传输结束
	DMA_Cmd(DMA2_Stream1,DISABLE);//关闭DMA2,Stream1
}
