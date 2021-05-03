#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>


#include "../include/uart.h"
#include "../include/common.h"

/*--------------------VARIABLES------------------*/
extern config_param_t config_uart;
pthread_t uart_read;
unsigned char start_cond = START_CONDITION_1;
unsigned char rcv_count = 0;
unsigned char nIndex = 0;
unsigned char length;
unsigned char uart_read_data[UART_DATA_SIZE];
unsigned char uart_read_data_actual[UART_DATA_SIZE];
int serial_port;


/*---------------Function Prototypes-------------*/
void* uart_read_task(void* arg);
void uart_write_task(void* data, unsigned int len);

/* Initializes the uart and initiates the uart read */
int uart_init()
{
	struct termios tty;

	printf("Uart Init\n");
	memset(&tty, 0, sizeof tty);
	serial_port = open(config_uart.port, O_RDWR | O_NOCTTY);
	/* Load the serial port settings to the termios struct tty */
	if (tcgetattr(serial_port, &tty) != 0)
	{
		printf("error %d from tcgetattr", errno);
		return 0;
	}

	/* no parity */
	tty.c_cflag &= ~PARENB;
	/* 1 stop bit */
	tty.c_cflag &= ~CSTOPB;
	/* clear number of bits field */
	tty.c_cflag &= ~CSIZE;
	/* 8 stop bits */
	tty.c_cflag |= CS8;
	/* disable hw flow control */
	tty.c_cflag &= ~CRTSCTS;
	tty.c_cflag |= CREAD|CLOCAL;

	/* disable sw flow control */
	tty.c_iflag &= ~(IXON | IXOFF | IXANY);
	/* disable special characters */
	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

	/* disable canonical mode */
	tty.c_lflag &= ~ICANON;
	/* disable echo */
	tty.c_lflag &= ~ECHO;

	/* disable erasure */
	tty.c_lflag &= ~ECHOE;
	/* disable new-line echo */
	tty.c_lflag &= ~ECHONL;
	/* disable INTR, QUIT, SUSP interpreation */
	tty.c_lflag &= ~ISIG;
	/* prevent interpreation of output bytes */
	tty.c_oflag &= ~OPOST;
	/* prevent conversion of newline to carriage return */
	tty.c_oflag &= ~ONLCR;

	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN]  = 1;

	/* set input and output baudrates to 9600 */
	cfsetispeed(&tty, config_uart.baudrate);
	cfsetospeed(&tty, config_uart.baudrate);

	/* Load the termios settings to serial port */
	if (tcsetattr(serial_port, TCSANOW, &tty) != 0)
	{
		printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	}
	
	/* Start Uart Read */
	printf("Create Thread for Uart Read\n");	
	if (pthread_create(&uart_read, NULL, uart_read_task, NULL))
	{
		perror("Uart Read\n");
		return 0;
	}

}

/* Thread Callback function for uart read */
void* uart_read_task(void* arg)
{
	int res;
	int loop;
	
	while(1)
	{
		/* Read UART Data */
		res = read(serial_port, uart_read_data + rcv_count, 1);
		if (res < 0)
		{
			perror("Uart read error : ");
			return 0 ;
		}
		rcv_count++;
		printf("UART Read: %c\n",uart_read_data[rcv_count - 1]);
		/* Parse data */
		/*
		 * Here Data is in the following format
		 * be<length N bytes><data N bytes>ad
		 * This is just an example data given.
		 * This can be changed as per your needs by modifying the MACROS
		 * START_BYTE_1, START_BYTE_2, END_BYTE_1 and END_BYTE_2
		 */

		
		if ((uart_read_data[rcv_count - 1] == START_BYTE_1) && (start_cond == START_CONDITION_1))
		{
			//printf("Start Condition_2\n");
			start_cond = START_CONDITION_2;
		}
		else if ((uart_read_data[rcv_count - 1] == START_BYTE_2) && (start_cond == START_CONDITION_2))
		{
			//printf("Start Condition_3\n");
			start_cond = START_CONDITION_3;
		}
		else if (start_cond == START_CONDITION_3)
		{
			length = uart_read_data[rcv_count -1];
			printf("Start Condition_4 Length : %d\n",length);
			start_cond = START_CONDITION_4;
		}
		else if (start_cond == START_CONDITION_4)
		{
			uart_read_data_actual[nIndex] = uart_read_data[rcv_count - 1];
			//printf("UART_DATA_ACTUAL: %c\n",uart_read_data_actual[nIndex]);
			nIndex++;
			if (rcv_count == length + 3)
			{
				printf("Start Condition_5\n");
				start_cond = START_CONDITION_5;
			}
		}
		else if ((uart_read_data[rcv_count - 1] == END_BYTE_1) && (start_cond == START_CONDITION_5))
		{
			//printf("Start Condition_6\n");
			start_cond = START_CONDITION_6;
		}
		else if ((uart_read_data[rcv_count - 1] == END_BYTE_2) && (start_cond == START_CONDITION_6))
		{
			uart_read_data_actual[nIndex++] = '\n';
			uart_read_data_actual[nIndex++] = '\r';
			//printf("Start Condition_7\n");
			start_cond = START_CONDITION_7;
			/*
			printf("Received Data: ");
			for (loop = 0; loop < length; loop++)
			{
				printf("%c.",uart_read_data_actual[loop]);
			}
			printf("\n");
			*/
			uart_write_task(uart_read_data_actual, nIndex);
			start_cond = START_CONDITION_1;
			rcv_count = 0;
			memset(uart_read_data, 0 , UART_DATA_SIZE);
			memset(uart_read_data_actual, 0 , UART_DATA_SIZE);
		}
		else
		{
			printf("Invalid format!! Provide the data in the following format 'be<1 byte length><data in length bytes>ad'\n");
			start_cond = START_CONDITION_1;
			rcv_count = 0;
			memset(uart_read_data, 0 , UART_DATA_SIZE);
			memset(uart_read_data_actual, 0 , UART_DATA_SIZE);
		}
		
	}
	
	pthread_exit(NULL);
}

/* Performs Uart Write Operation */
void uart_write_task(void* data, unsigned int len)
{
	printf("UART Write Initiated\n");
	int res;
	res = write(serial_port, data, len);
	if (res < 0)
	{
		perror("Uart Write Error: ");
	}

}
