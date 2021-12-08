/******************************************************************************
 *
 * Module: UART
 *
 * File Name: uart.c
 *
 * Description: Source file for the UART driver
 *
 * Author: Habiba Hamed
 *
 *******************************************************************************/
#include "uart.h"
#include "avr/io.h" /* To use the UART Registers */
#include "common_macros.h" /* To use the macros like SET_BIT */

/*******************************************************************************
 *                      Functions Definitions                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const Uart_ConfigType * Config_Ptr){
	uint8 factor;
	uint64 boudRatePrescaler;

	/************************** UCSRA Description **************************
	 * Bit 1=U2X
	 *U2X=1 =>> Asynchronous double speed mode
	 *U2X=0 =>> Asynchronous normal mode
	 ***********************************************************************/
	UCSRA=(UCSRA&(0xF2))|((Config_Ptr->mode)<<U2X);

	/************************** UCSRB Description **************************
	 * RXCIE = 0 Disable USART RX Complete Interrupt Enable
	 * TXCIE = 0 Disable USART Tx Complete Interrupt Enable
	 * UDRIE = 0 Disable USART Data Register Empty Interrupt Enable
	 * RXEN  = 1 Receiver Enable
	 * RXEN  = 1 Transmitter Enable
	 ***********************************************************************/
	UCSRB = (1<<RXEN) | (1<<TXEN);

	/************************** UCSRC Description **************************
	 * the URSEL must be one when writing the UCSRC
	 * the UMSEL =0 =>> Asynchronous operation
	 * Bit 5-4: UMP1:0, these bits enable and set the type of parity generation and check
	 *UMP1=0 ,UMP0=0 =>> disabled
	 *UMP1=1 ,UMP0=0 =>> Enabled, Even parity
	 *UMP1=1 ,UMP0=1 =>> Enabled, Odd parity
	 * Bit 3: USBS, this bit selects the number of stop bits to be inserted by the transmitter
	 *USBS=0 =>> 1-bit
	 *USBS=1 =>> 2-bit
	 * Bit 2:1: UCSZ1:0 ,these two bits in combination with the UCSZ2 bit in UCSRB register helps
		 choosing the number of data bits in your frame
	 *UCSZ0=0 ,UCSZ1=0 ,UCSZ2=0 =>> 5-bit
	 *UCSZ0=1 ,UCSZ1=0 ,UCSZ2=0 =>> 6-bit
	 *UCSZ0=0 ,UCSZ1=1 ,UCSZ2=0 =>> 7-bit
	 *UCSZ0=1 ,UCSZ1=1 ,UCSZ2=0 =>> 8-bit
	 *UCSZ0=1 ,UCSZ1=1 ,UCSZ2=1 =>> 5-bit
	 * Bit 0: UCPOL, this bit is used for Synchronous mode only
	 *UCPOL=0  =>> Asynchronous mode is used

	 ***********************************************************************/
	UCSRC |= (1<<URSEL);
	UCSRC =(UCSRC&(0xCF))|((Config_Ptr->parity)<<UPM0);
	UCSRC =(UCSRC&(0xCF))|((Config_Ptr->stop)<<USBS);
	if(Config_Ptr->size==7){
		UCSRC =(UCSRC&(0xCF))|(((Config_Ptr->stop)-4)<<UCSZ0);
		UCSRB |=(1<<UCSZ2);
	}
	else {
		UCSRC =(UCSRC&(0xCF))|((Config_Ptr->stop)<<UCSZ0);
	}
	switch (Config_Ptr->mode){
	case 0:
		factor=16;
		break;
	case 1:
		factor=8;
		break;
	}
	boudRatePrescaler=((F_CPU)/(factor*Config_Ptr->boudRate))-1;
	UBRRH = boudRatePrescaler>>8;
	UBRRL = boudRatePrescaler;
}
/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data){

	/*
	 * UDRE flag is set when the Tx buffer (UDR) is empty and ready for
	 * transmitting a new byte so wait until this flag is set to one
	 */
	while(BIT_IS_CLEAR(UCSRA,UDRE)){}

	/*
	 * Put the required data in the UDR register and it also clear the UDRE flag as
	 * the UDR register is not empty now
	 */
	UDR = data;
}

/*
 * Description :
 * Function which is responsible for receive byte from another UART device.
 */
uint8 UART_receiveByte(void){
	/* RXC flag is set when the UART receive data so wait until this flag is set to one */
	while(BIT_IS_CLEAR(UCSRA,RXC)){}

	/*
	 * Read the received data from the Rx buffer (UDR)
	 * The RXC flag will be cleared after read the data
	 */
	return UDR;
}
/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
/*void UART_sendString(const uint8 *Str){
	uint8 i = 0;

	/* Send the whole string */
	//while(Str[i] != '\0')
	//{
	//	UART_sendByte(Str[i]);
	//	i++;
	//}
//}

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
//void UART_receiveString(uint8 *Str){
	//uint8 i = 0;

	/* Receive the first byte */
	//Str[i] = UART_receiveByte();

	/* Receive the whole string until the '#' */
	//while(Str[i] != '#')
	//{
	//	i++;
	//	Str[i] = UART_receiveByte();
	//}

	/* After receiving the whole string plus the '#', replace the '#' with '\0' */
	//Str[i] = '\0';
//}

