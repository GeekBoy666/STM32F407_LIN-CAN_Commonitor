#include "eta.h" 
#include "delay.h"		 

/*********************************************************************
*
*       ETA Channel To PIN Index
*/
const static uint16_t ETA_Channel_To_PIN[] = 
{
	GPIO_Pin_0,	
	GPIO_Pin_1,
	GPIO_Pin_2,
	GPIO_Pin_3,
	GPIO_Pin_4,
	GPIO_Pin_5,
	GPIO_Pin_6,
	GPIO_Pin_7,
	GPIO_Pin_8,
	GPIO_Pin_9,
	GPIO_Pin_10,
	GPIO_Pin_11,
	GPIO_Pin_12,
	GPIO_Pin_13,
	GPIO_Pin_14,
	GPIO_Pin_15
};

/*********************************************************************
*
*       ETA_Init
*	
*/
/*Ϊ��ʵ�ֽ�ĳһͨ�����ó�������������GPIOӳ�䵽ͨ����*/
void ETA_Init(int Chn_num)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	
	if ( Chn_num > 29 && Chn_num < 32 )
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num - 17];	// CH31 - CH30    PIN14 - PIN13
		GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	}
	if ( Chn_num > 24 && Chn_num < 30 )
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num - 18];	// CH29 - CH25    PIN11 - PIN07
		GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	}
	if ( Chn_num > 15 && Chn_num < 25 )
	{	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOGʱ��
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num - 16];	// CH24 - CH16  PIN08 -PIN00
		GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
	}
	if( Chn_num > 9 && Chn_num < 16 )
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOCʱ��
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num - 10];	// CH15 - CH10   PIN05 - PIN00
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	}
	if ( Chn_num == 8 || Chn_num == 9 )
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num - 8];	// CH09 - CH08   PIN01 - PIN00  
		GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��	
	}
	if ( Chn_num < 8 )
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num];	// CH07 - CH00       PIN07 - PIN00
		GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	}
}
/*********************************************************************
*
*       Adc_Init
*	
*/
void Adc_Init(int Chn_num)
{    
  GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ��ADC1ʱ��
  //��ʼ��ADC1ͨ��Ϊģ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
	
	if (Chn_num < 8)
	{
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num];	// ADCCH07 - ADCCH00 			PIN07 - PIN00
		GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��  
	}
	if ( Chn_num == 8 || Chn_num == 9)
	{
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num - 8];	// ADCCH09 - ADCCH08			PIN01 - PIN00
		GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��		
	}
	if( Chn_num > 9 && Chn_num < 16)
	{
		GPIO_InitStructure.GPIO_Pin = ETA_Channel_To_PIN[Chn_num - 10];	// ADCCH15 - ADCCH10 			PIN05 - PIN00
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
	}
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����	 
 
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
  ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
	
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
  ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
  ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
	
	ADC_Cmd(ADC1, ENABLE);//����ADת����	
}				  
//���ADCֵ
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_15
//����ֵ:ת�����
u16 Get_Adc(u8 ch)   
{
	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    
  
	ADC_SoftwareStartConv(ADC1);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}
//��ȡͨ��ch��ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//ch:ͨ�����
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 
