#include "lin.h"
#include "delay.h"
#include "usart.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

u8 LIN_CHECKSUM_PID = 0;
u8 LIN_DLC = 0;
unsigned char Flag_DataReady1 = 0;
unsigned char Flag_DataReady2 = 0;
unsigned char ReciveBKTimes = 0;
char  LIN_RXD_buf1[33] = {NULL};	
char  LIN_RXD_buf2[33] = {NULL};
unsigned int k = 0;	

void LIN_Slave_int(int baud_rate)
{
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);       // ʹ��PORTCʱ��  ���ڿ���PC12 LIN TXD 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);       // ʹ��PORTDʱ��  ���ڿ���PD2 LIN RXD 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);       // ʹ��LINʱ��

	USART_DeInit(UART5);	//  ��λ����5
	
  // USART5_RX PD2
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;    							//  PD2 = LIN RXD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                //  ���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //  �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;          //  100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //  ����
  GPIO_Init(GPIOD, &GPIO_InitStructure);                      //  ��ʼ��PD2
  // USART5_TX PC12       
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;    							//  PC12 = LIN TXD
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                //  ���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //  �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;          //  100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                //  ����
  GPIO_Init(GPIOC, &GPIO_InitStructure);                      //  ��ʼ��PC12
        
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12, GPIO_AF_UART5);   //  PC12 = LIN TXD
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2, GPIO_AF_UART5);   //  PD2 = LIN RXD
    //UART5 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority= 3 ;	//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;			//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                      //����ָ���Ĳ�����ʼ��NVIC�Ĵ���
  
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = baud_rate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;			//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx;					//�ӻ�ֻ��ģʽ

  USART_Init(UART5, &USART_InitStructure); 					//��ʼ������
	
	USART_LINBreakDetectLengthConfig(UART5, USART_LINBreakDetectLength_10b);
  USART_ITConfig(UART5, USART_IT_LBD , ENABLE);	  //����break�ж�  �ӻ�����Ҫ�˹���
	USART_LINCmd( UART5, ENABLE);  

	USART_ITConfig(UART5, USART_IT_RXNE , ENABLE);			//�����ж�	
	USART_Cmd(UART5, ENABLE);                    				//ʹ�ܴ���	


}
//------------------------------------------------------------------����3�жϷ������
void UART5_IRQHandler(void)                				
{
	int Res;
	char temp[3];
	int j;
#if SYSTEM_SUPPORT_OS   // ʹ��UCOS����ϵͳ
		OSIntEnter();    
#endif

	if( USART_GetITStatus(UART5,USART_IT_RXNE) != RESET)   //�ֽڿ�ʼ����
	{			
		if (ReciveBKTimes == 1)	//�յ�BK����Ϊ����ʱ
		{	
			Res =	USART_ReceiveData(UART5);						   // ���յ����ֽ� 
			sprintf(temp,"%02X ",Res);
			for(j = 0; j < 3; j++)
			{
				LIN_RXD_buf1[k] = temp[j];
				k = k+1;
				if( k >= 33)
				{
					k = 0;
					Flag_DataReady1 = 1;
				}
			}				
		}
		else if (ReciveBKTimes == 0)		//�յ�break�źŸ���Ϊż��
		{
			Res =	USART_ReceiveData(UART5);						    // ���յ����ֽ�      			
			sprintf(temp,"%02X ",Res);
			for(j = 0; j < 3; j++)
			{
				LIN_RXD_buf2[k] = temp[j];
				k = k+1;
				if( k >= 33)
				{
					k = 0;
					Flag_DataReady2 = 1;
				}
			}				
		}			
	}	
	
	if( USART_GetITStatus(UART5,USART_IT_LBD) != RESET)   // �����⵽break�ź�
	{		
		k = 0;
		ReciveBKTimes = ReciveBKTimes + 1;
		switch(ReciveBKTimes)
		{
			case 1:
				Flag_DataReady1 = 0;
				Flag_DataReady2 = 1;
				break;
			case 2:
				ReciveBKTimes = 0;
				Flag_DataReady1 = 1;							
				Flag_DataReady2 = 0;
			break;
			default:
				break;
		}
		USART_ClearITPendingBit( UART5, USART_IT_LBD );//���BK��־		
	}	
#if SYSTEM_SUPPORT_OS  
    OSIntExit();    	// �˳��ж�
#endif
}
