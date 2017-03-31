#include "key.h"
#include "led.h"

uint16_t keyPressTimeCount = 0;

void key_timer_init(void);
void stop_key_timer(void);
void start_key_timer(void);

/***************************************
**����Io�ڳ�ʼ��
**********************************/
void key_init(void) //IO��ʼ��
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ�ܰ������ʱ��
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //LED0��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	key_timer_init();
}

/**************************************
**�ϵ���
***************************************/
void power_on_check(void)
{
	if(IS_KEY_DOWN)//����������
	{
		start_key_timer();
		while(1)
		{
			if(keyPressTimeCount >= 100)//��������һ����
			{
				stop_key_timer();
				keyPressTimeCount = 0;
				POWER_ON;
				LED0 = 0;
				break;
			}
		}
	}
}

void power_off_check(void)
{

	if(IS_KEY_DOWN)//����������
	{
		start_key_timer();
		while(1)
		{
			if(keyPressTimeCount >= 100)//��������һ����
			{
				stop_key_timer();
				keyPressTimeCount = 0;
				POWER_OFF;
				LED0 = 0;
				break;
			}
		}
	}
}


/***************************************
**����10ms��ʱ��
**********************************/
void key_timer_init(void) 
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 9; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig( 		 	//ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM4, 						//TIM4
		TIM_IT_Update,     //TIM �ж�Դ  //TIM �����ж�Դ 
		DISABLE 					
		);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  						//TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  			//�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				 //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM4, DISABLE);  
}


void stop_key_timer(void)
{
	TIM_ITConfig( 		      	//�رն�ʱ�ж�
				TIM4, 						 
				TIM_IT_Update,      
				DISABLE 					
				);
				TIM_Cmd(TIM4, DISABLE);   //�ر�TIM4
}

void start_key_timer(void)
{
	 TIM_ITConfig( 		      	//������ʱ�ж�
				TIM4, 						 
				TIM_IT_Update,      
				ENABLE 					
				);
				TIM_Cmd(TIM4, ENABLE);   //����TIM4
}


/*******************************************
**������ʱ���жϷ������
********************************************/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		keyPressTimeCount++;
	}
}









