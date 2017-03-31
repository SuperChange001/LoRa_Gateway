#include "timer.h"

/*******************************************************
**ͨ�ö�ʱ���жϳ�ʼ��
**����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
**arr���Զ���װֵ��          ��ʱ��3Tclk = 72Mhz
**psc��ʱ��Ԥ��Ƶ��
**����ʹ�õ��Ƕ�ʱ��2!       Tout = (arr+1*psc+1)/Tclk
********************************************************/
void Timerx_Init(u16 arr,u16 psc)//
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig( 		 	//ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM3, 						//TIM3
		TIM_IT_Update,     //TIM �ж�Դ  //TIM �����ж�Դ 
		ENABLE  					//ʹ��
		);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  						//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  			//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				 //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                          //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����					 
}

/*******************************************
**��ʱ���жϷ������
********************************************/
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );    //���TIMx���жϴ�����λ:TIM �ж�Դ 
		//usart3_send_buf(29,sendToBleData);               //�������ݵ�����
	}
}












