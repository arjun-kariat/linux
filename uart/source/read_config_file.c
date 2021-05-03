#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "../include/read_config_file.h"
#include "../include/common.h"

/*-----------------MACROS------------------------*/
#define PATH_CONFIG_FILE "config/config.txt"


/*--------------------VARIABLES------------------*/
config_param_t config_uart;
unsigned char read_line[100];
unsigned char param_port[100];
unsigned char param_baud[100];

unsigned int baudMap[10][2] = {{1200, B1200},
                               {2400, B2400},
                               {4800, B4800},
                               {9600, B9600},
                               {19200, B19200},
                               {38400, B38400},
                               {57600, B57600},
                               {115200, B115200},
                               {230400, B230400},
                               {460800, B460800},
                              };

/*---------------Function Prototypes-------------*/
unsigned int getBaudrateEnum(unsigned int baud);

/* Read UART Config Data from the config file */
void read_config_data()
{
	FILE *fp;
	
	fp = fopen(PATH_CONFIG_FILE,"r");
	if (fp == NULL)
	{
		printf("File open error\n");
		return;
	}

	/* Read first line */
	fgets(read_line, 100, fp);
	strcpy(param_port, read_line + 5);
	/* Removing the \n from the end of the string */
	param_port[strlen(param_port) - 1] = 0;
	strcpy(config_uart.port, param_port);
	//printf("First para : %s\n", config_uart.port);
	
	fgets(read_line, 100, fp);
	strcpy(param_baud, read_line + 9);
	/* Removing the \n from the end of the string */
	param_baud[strlen(param_baud) - 1] = 0;
	config_uart.baudrate = getBaudrateEnum(atoi(param_baud));
	//printf("Second para: %d\n",config_uart.baudrate);
	
	
	fclose(fp);
}

/* Using the baudrate taken from the config file take the corresponding baudrate MACRO */
unsigned int getBaudrateEnum(unsigned int baud)
{
	unsigned int baudRateEnum;
	unsigned char nIndex = 0xff;
	for (nIndex = 0; nIndex < sizeof(baudMap) / (sizeof(unsigned int) * 2); nIndex++)
	{
		if (baud == baudMap[nIndex][0])
		{
			printf("Found Baud: %d\n",baudMap[nIndex][0]);
			break;
		}
	}
	if (nIndex == 0xff)
	{
		printf("Invalid baud Found!!Returning default baud as 9600\n");
		return B9600;
	}
	else
	{
		return baudMap[nIndex][1];
	}
}
