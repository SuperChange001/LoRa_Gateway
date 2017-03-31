#include "usart1.h"
#include "stdio.h"
#include "gps.h"

/*-------------------------------------------------*
����һ PA10-RX --->  GPS TX
			 PA9- TX --->  GPS RX
����GPS��Ϣ 10Hz���,������Ϊ9600,�����ʽ���л��з�
*--------------------------------------------------*/

u8 USART1_RX_BUF[USART1_MAX_RECV_LEN];     //���ջ���,���64���ֽ�

u8 TEMP_BUF[USART1_MAX_RECV_LEN];          //���ջ���
nmea_msg gpsx;                            //����������gps��Ϣ

void usart1_recieve_task(void);

/*--------------------------------------------*/
//����״̬
//bit15��������ɱ�־
//bit14�����յ�0x0d
//bit13~0�����յ�����Ч�ֽ���Ŀ
/*--------------------------------------------*/
u16 USART1_RX_STA=0;   //����״̬���


void usart1_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	
	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	//Usart1 NVIC ����

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1

	//USART1��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);


	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�

	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}

/*********************************************************
**����1�жϷ���
*********************************************************/
void USART1_IRQHandler(void)            
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART1_RX_STA&0x8000)==0)//����δ���
		{
			if(USART1_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
				else USART1_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
				{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_MAX_RECV_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   	
		usart1_recieve_task();
		
	 } 
} 


/**************************************************************
**����GPS��Ϣ������
***************************************************************/
void usart1_recieve_task(void)
{
	u16 len = 0;
	u16 t = 0;
	if(USART1_RX_STA&0x8000)
	{					   
		len=USART1_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
		for(t=0;t<len;t++) 
		{
			TEMP_BUF[t]=USART1_RX_BUF[t];
		}
		USART1_RX_STA=0;
		TEMP_BUF[t]=0;				
		GPS_Analysis(&gpsx,(u8*)TEMP_BUF);//����gps����,�õ���γ�Ⱥ��ٶ�
		

		
		
	}
}




