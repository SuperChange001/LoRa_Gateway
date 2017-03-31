#ifndef __USART1_H
#define __USART1_H
#include "stm32f10x.h"

#define USART1_MAX_RECV_LEN   128

void usart1_init(u32 bound);
void usart1_recieve_task(void);

#endif
