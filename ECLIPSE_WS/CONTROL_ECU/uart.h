 /******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.h
 *
 * Description: Header file for the UART driver
 *
 * Author: Habiba Hamed
 *
 *******************************************************************************/


#ifndef UART_H_
#define UART_H_
#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	ASYNCHRONOUS_NORMAL_MODE,ASYNCHRONOUS_DOUBLE_SPPED_MODE
}UART_Mode;
typedef enum{
	FIVE,SIX,SEVEN,EIGHT,NINE=7
}Uart_CharacterSize;
typedef enum{
	DISABLED,ENABLED_EVEN=2,ENABLED_ODD
}UART_ParityBitMode;
typedef enum{
	ONE,TWO
}Uart_StopBitSelect;
typedef struct{
	UART_Mode mode;
	Uart_CharacterSize size;
	UART_ParityBitMode parity;
	Uart_StopBitSelect stop;
	uint64  boudRate;

}Uart_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const Uart_ConfigType * Config_Ptr);

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
