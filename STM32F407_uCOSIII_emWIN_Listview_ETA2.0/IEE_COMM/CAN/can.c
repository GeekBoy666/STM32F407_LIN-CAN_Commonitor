#include "can.h"
#include "delay.h"
#include "usart.h"
#include "sys.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//CAN���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0 
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   ��Χ:CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024; tq=(brp)*tpclk1
//������=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
//mode:CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ42M,�������CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_LoopBack);
//������Ϊ:42M/((6+7+1)*6)=500Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��; 

unsigned char Flag_CANDataReady = 0;
char CAN_RXD_buf[24] = {NULL};

u8 CAN1_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

  GPIO_InitTypeDef GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN1_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif
    // ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);   //  ʹ��PORTBʱ��	                   											 

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);    //  ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;            //  ���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //  �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //  100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //  ����
  GPIO_Init(GPIOB, &GPIO_InitStructure);                  //  ��ʼ��PB8 ,PB9
	
	// ���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1);   //  PB8����ΪCAN1      lzg
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1);   //  PB9����ΪCAN1      lzg
	  
  	// CAN��Ԫ����
  CAN_InitStructure.CAN_TTCM=DISABLE;	                    //  ��ʱ�䴥��ͨ��ģʽ   
	CAN_InitStructure.CAN_ABOM=DISABLE;	                    //  ����Զ����߹���	  
	CAN_InitStructure.CAN_AWUM=DISABLE;                     //  ˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
	CAN_InitStructure.CAN_NART=ENABLE;	                    //  ��ֹ�����Զ����� 
	CAN_InitStructure.CAN_RFLM=DISABLE;	                    //  ���Ĳ�����,�µĸ��Ǿɵ�  
  CAN_InitStructure.CAN_TXFP=DISABLE;	                    //  ���ȼ��ɱ��ı�ʶ������ 
  CAN_InitStructure.CAN_Mode= mode;	                    //  ģʽ���� 
  CAN_InitStructure.CAN_SJW=tsjw;	                        //  ����ͬ����Ծ���(Tsjw)Ϊtsjw+1��ʱ�䵥λ CAN_SJW_1tq~CAN_SJW_4tq
  CAN_InitStructure.CAN_BS1=tbs1;                         //  Tbs1��ΧCAN_BS1_1tq ~CAN_BS1_16tq
  CAN_InitStructure.CAN_BS2=tbs2;                         //  Tbs2��ΧCAN_BS2_1tq ~	CAN_BS2_8tq
  CAN_InitStructure.CAN_Prescaler=brp;                    //  ��Ƶϵ��(Fdiv)Ϊbrp+1	
  CAN_Init(CAN1, &CAN_InitStructure);                     //  ��ʼ��CAN1 
    
	// ���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	                        //  ������0
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;      //  32λ 
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;                    //  32λID
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;                //  32λMASK
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;  //  ������0������FIFO0
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;                //  ���������0
  CAN_FilterInit(&CAN_FilterInitStructure);                           //  �˲�����ʼ��
		
#if CAN1_RX0_INT_ENABLE
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;           // �����ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;                  // �����ȼ�Ϊ2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN1_RX0_INT_ENABLE	//ʹ��RX0�ж�
//----------------------------------------------------------------------�жϷ�����			    
void CAN1_RX0_IRQHandler(void)
{
	int i=0;
	char temp[3];
	int j;
	int k = 0;
	CanRxMsg RxMessage;

#if SYSTEM_SUPPORT_OS   // ʹ��UCOS����ϵͳ
  OSIntEnter();    
#endif
  CAN_Receive(CAN1, 0, &RxMessage);
	for(i=0;i<RxMessage.DLC;i++)
	{
		sprintf(temp,"%02X ",RxMessage.Data[i]);
		for(j = 0; j < 3; j++)
		{
			CAN_RXD_buf[k] = temp[j];
			k = k+1;
			if(k == 20)
			{	Flag_CANDataReady = 1;}
		}
	}
	Flag_CANDataReady = 1;
#if SYSTEM_SUPPORT_OS  
    OSIntExit();    	// �˳��ж�
#endif  
}
#endif

//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//		 ����,���յ����ݳ���;
u8 CAN1_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    
  if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)
		return 0;		//û�н��յ�����,ֱ���˳� 
    
  CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	 
  for(i=0;i<RxMessage.DLC;i++)
		buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}














