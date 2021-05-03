#include <stdio.h>

#include "../include/read_config_file.h"
#include "../include/common.h"
#include "../include/uart.h"

extern config_param_t config_uart;

int main()
{
	/* Read the uart config params from the file */
	read_config_data();
	printf("First para : %s\n", config_uart.port);
	printf("Second para: %d\n",config_uart.baudrate);

	uart_init();
	while(1);
}
