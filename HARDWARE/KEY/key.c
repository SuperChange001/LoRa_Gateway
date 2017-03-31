#include "key.h"
#include "led.h"

uint16_t keyPressTimeCount = 0;

void key_timer_init(void);
void stop_key_timer(void);
void start_key_timer(void);

/***************************************
**按键Io口初始化
**********************************/
void key_init(void) //IO初始化
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能按键输出时钟
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     //LED0引脚配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	key_timer_init();
}

/**************************************
**上电检测
***************************************/
void power_on_check(void)
{
	if(IS_KEY_DOWN)//按键按下了
	{
		start_key_timer();
		while(1)
		{
			if(keyPressTimeCount >= 100)//长按超过一秒了
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

	if(IS_KEY_DOWN)//按键按下了
	{
		start_key_timer();
		while(1)
		{
			if(keyPressTimeCount >= 100)//长按超过一秒了
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
**按键10ms定时器
**********************************/
void key_timer_init(void) 
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 9; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig( 		 	//使能或者失能指定的TIM中断
		TIM4, 						//TIM4
		TIM_IT_Update,     //TIM 中断源  //TIM 触发中断源 
		DISABLE 					
		);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  						//TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  			//从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 				 //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM4, DISABLE);  
}


void stop_key_timer(void)
{
	TIM_ITConfig( 		      	//关闭定时中断
				TIM4, 						 
				TIM_IT_Update,      
				DISABLE 					
				);
				TIM_Cmd(TIM4, DISABLE);   //关闭TIM4
}

void start_key_timer(void)
{
	 TIM_ITConfig( 		      	//开启定时中断
				TIM4, 						 
				TIM_IT_Update,      
				ENABLE 					
				);
				TIM_Cmd(TIM4, ENABLE);   //开启TIM4
}


/*******************************************
**按键定时器中断服务程序
********************************************/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		keyPressTimeCount++;
	}
}









