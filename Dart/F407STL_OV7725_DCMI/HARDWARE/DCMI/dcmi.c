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
	if(DMA_GetITStatus(DMA2_Stream1,DMA_IT_TCIF1)==SET)//�д������
    {
        DMA_ClearITPendingBit(DMA2_Stream1,DMA_IT_TCIF1);
        
        dma_times++;
    }
}


void DCMI_IRQHandler(void)
{
    if(DCMI_GetITStatus(DCMI_IT_FRAME)==SET)    //֡�ж�
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

//DCMI DMA����
//DMA_Memory0BaseAddr:�洢����ַ
//DMA_BufferSize:�洢������    0~65535
//DMA_MemoryDataSize:�洢��λ��
//DMA_MemoryDataSize:�洢��λ��
//DMA_MemoryInc:�洢��������ʽ
void DCMI_DMA_Init(u32 DMA_Memory0BaseAddr,u16 DMA_BufferSize,u32 DMA_MemoryDataSize)
{ 
	DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2ʱ��ʹ��
	DMA_DeInit(DMA2_Stream1);
	while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������
	
  DMA_InitStructure.DMA_Channel = DMA_Channel_1;  //ͨ��1 DCMIͨ��
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&DCMI->DR;//�����ַΪ:DCMI->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = DMA_Memory0BaseAddr;//DMA �洢��0��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//���赽�洢��ģʽ
  DMA_InitStructure.DMA_BufferSize = DMA_BufferSize;//���ݴ�����
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//�������ݳ���:32λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize;//�洢�����ݳ���
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// ʹ��ѭ��ģʽ 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//�����ȼ�
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable; //FIFOģʽ        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//ʹ��ȫFIFO 
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//����ͻ�����δ���
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//�洢��ͻ�����δ���
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);//��ʼ��DMA Stream
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Stream1,DMA_IT_TC,ENABLE);//������������ж�
}

void My_DCMI_Init(void)
{
    DCMI_InitTypeDef DCMI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI,ENABLE);//ʹ��DCMIʱ��
    
    #ifdef F407IG
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOH|RCC_AHB1Periph_GPIOI,ENABLE);//ʹ��GPIOA/C/E/Iʱ��
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //���ù������
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    
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
	
	DCMI_DeInit();//���ԭ�������� 
    
    DCMI_InitStructure.DCMI_CaptureMode=DCMI_CaptureMode_Continuous;//����ģʽ
	DCMI_InitStructure.DCMI_CaptureRate=DCMI_CaptureRate_All_Frame;//ȫ֡����
	DCMI_InitStructure.DCMI_ExtendedDataMode= DCMI_ExtendedDataMode_8b;//8λ���ݸ�ʽ
	DCMI_InitStructure.DCMI_SynchroMode= DCMI_SynchroMode_Hardware;//Ӳ��ͬ��HSYNC,VSYNC
	DCMI_InitStructure.DCMI_VSPolarity=DCMI_VSPolarity_High;//VSYNC �ߵ�ƽ��Ч
	DCMI_InitStructure.DCMI_HSPolarity= DCMI_HSPolarity_Low;//HSYNC �͵�ƽ��Ч//Low
	DCMI_InitStructure.DCMI_PCKPolarity= DCMI_PCKPolarity_Rising;//PCLK ��������Ч//Rising
	DCMI_Init(&DCMI_InitStructure);
    
    //DCMI_IT_LINE|DCMI_IT_FRAME
	DCMI_ITConfig(DCMI_IT_LINE|DCMI_IT_FRAME,ENABLE);
	
	DCMI_Cmd(ENABLE);	//DCMIʹ��
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

}

void DCMI_Start(void)
{
	DMA_Cmd(DMA2_Stream1, ENABLE);//����DMA2,Stream1
	DCMI_CaptureCmd(ENABLE);//DCMI����ʹ��
}

void DCMI_Stop(void)
{
    DCMI_CaptureCmd(DISABLE);//DCMI����ʹ�ر�
	while(DCMI->CR&0X01);		//�ȴ��������
	DMA_Cmd(DMA2_Stream1,DISABLE);//�ر�DMA2,Stream1
}
