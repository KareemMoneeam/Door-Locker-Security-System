/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART AVR driver
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "std_types.h"

#define MC1_READY   	   0x10
#define MC2_READY   	   0x20

#define F_CPU 8000000UL


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/

typedef enum
{
	FIVE_BITS_DATA, SIX_BITS_DATA, SEVEN_BITS_DATA, EIGHT_BITS_DATA, NINE_BITS_DATA=7
}UART_BitData;

typedef enum
{
	DISABLED, EVEN_PARITY=2, ODD_PARITY=3
}UART_Parity;

typedef enum {
	ONE_STOP_BIT, TWO_STOP_BIT
}UART_StopBit;

typedef enum {
	BAUD_RATE_10_BPS    =   10,
	BAUD_RATE_300_BPS   =   300,
	BAUD_RATE_600_BPS   =   600,
	BAUD_RATE_1200_BPS  =   1200,
	BAUD_RATE_2400_BPS  =   2400,
	BAUD_RATE_4800_BPS  =   4800,
	BAUD_RATE_9600_BPS  =   9600,
	BAUD_RATE_14400_BPS =   14400,
	BAUD_RATE_19200_BPS =   19200,
	BAUD_RATE_38400_BPS =   38400,
	BAUD_RATE_57600_BPS =   57600,
	BAUD_RATE_115200_BPS=   115200,
	BAUD_RATE_128000_BPS=   128000,
	BAUD_RATE_256000_BPS=   256000
}UART_BaudRate;

typedef struct{
	UART_BitData  bit_data;
	UART_Parity   parity;
	UART_StopBit  stop_bit;
	UART_BaudRate baud_rate;
}UART_ConfigType;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType * Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
