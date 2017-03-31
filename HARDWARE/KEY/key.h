#ifndef _KEY_H__
#define _KEY_H__
#include "stm32f10x.h"

#define READ_KEY0 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)

#define        IS_KEY_DOWN        ( READ_KEY0 == 0 ) //��������
#define        IS_KEY_UP          ( READ_KEY0 == 1 ) //�����ſ�

#define        POWER_ON            GPIO_WriteBit(GPIOB,GPIO_Pin_2,Bit_SET)
#define        POWER_OFF           GPIO_WriteBit(GPIOB,GPIO_Pin_2,Bit_RESET)

void power_on_check(void);
void key_init(void);//IO��ʼ��
void power_off_check(void);

#endif
