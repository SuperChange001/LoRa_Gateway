#include "usart1.h"
#include "stdio.h"
#include "gps.h"

/*-------------------------------------------------*
串口一 PA10-RX --->  GPS TX
			 PA9- TX --->  GPS RX
接收GPS信息 10Hz输出,波特率为9600,输出格式会有换行符
*--------------------------------------------------*/

u8 USART1_RX_BUF[USART1_MAX_RECV_LEN];     //接收缓冲,最大64个字节

u8 TEMP_BUF[USART1_MAX_RECV_LEN];          //接收缓存
nmea_msg gpsx;                            //解析出来的gps信息

void usart1_recieve_task(void);

/*--------------------------------------------*/
//接收状态
//bit15，接收完成标志
//bit14，接收到0x0d
//bit13~0，接收到的有效字节数目
/*--------------------------------------------*/
u16 USART1_RX_STA=0;   //接收状态标记


void usart1_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//Usart1 NVIC 配置

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1

	//USART1初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断

	USART_Cmd(USART1, ENABLE);                    //使能串口 

}

/*********************************************************
**串口1中断服务
*********************************************************/
void USART1_IRQHandler(void)            
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART1_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART1_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
				else USART1_RX_STA|=0x8000;	//接收完成了 
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_MAX_RECV_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   	
		usart1_recieve_task();
		
	 } 
} 


/**************************************************************
**接收GPS信息并解析
***************************************************************/
void usart1_recieve_task(void)
{
	u16 len = 0;
	u16 t = 0;
	if(USART1_RX_STA&0x8000)
	{					   
		len=USART1_RX_STA&0x3fff;//得到此次接收到的数据长度
		for(t=0;t<len;t++) 
		{
			TEMP_BUF[t]=USART1_RX_BUF[t];
		}
		USART1_RX_STA=0;
		TEMP_BUF[t]=0;				
		GPS_Analysis(&gpsx,(u8*)TEMP_BUF);//解析gps数据,得到经纬度和速度
		

		
		
	}
}




