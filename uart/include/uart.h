#ifndef __UART_H__
#define __UART_H__

/* This includes the 2 bytes of Start bytes, 1 byte length and 2 bytes of End bytes */
#define UART_DATA_SIZE 100

/* Start and End bytes */
#define START_BYTE_1 'b'
#define START_BYTE_2 'e'
#define END_BYTE_1 'a'
#define END_BYTE_2 'd'

/* States of Uart Reception */
enum START_CONDITION
{
	START_CONDITION_1 = 0,
	START_CONDITION_2 = 1,
	START_CONDITION_3 = 2,
	START_CONDITION_4 = 3,
	START_CONDITION_5 = 4,
	START_CONDITION_6 = 5,
	START_CONDITION_7 = 6
};


int uart_init();

#endif//__UART_H__
