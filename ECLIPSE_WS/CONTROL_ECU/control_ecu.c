/*******************************************************************************
 * module:   CONTROL_ECU
 *
 * File Name: control_ecu.c
 *
 * Description: Source file for Door locker security system application CONTROL_ECU
 *
 * Created on: 28‏/10‏/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/
#include "timer.h" /* to count events time  */
#include "uart.h"  /* to communicate with HMI_ECU*/
#include "external_eeprom.h" /* to store password in EEPROM */
#include "buzzer.h"          /* to make an alert in case of theft */
#include "dc_motor.h"        /* to control the door  */
#include "std_types.h"       /* to use the standard types */
#include "control_ecu.h"
#include <avr/io.h>          /* to use AVR registers like SREG*/
#include <util/delay.h>
/************************************************************************************************
 *                                GLOBAL VARIABLES                                              *
 ************************************************************************************************/
uint8 g_password[PASSWORD_SIZE];         /*global array to store the first password entered in it */
uint8 g_reenterdPassword[PASSWORD_SIZE]; /*global array to store the Re-entered password entered in it */
uint8 g_savedPassword[PASSWORD_SIZE];    /*global array to store the saved password entered in it */
uint8 g_command;                         /*global variable to store the commands in it */
uint8 g_wrong=0;                         /*global variable to count wrong password entered times*/
volatile uint8 g_tick=0;                 /*global ticks to count timer interrupts and been volatile to
                                          be able to used in ISR code */
volatile uint8 g_secondsCounter=0;
/************************************************************************************************
 *                                 UART & TIMER CONFIGURATIONS                                  *
 ************************************************************************************************/
/*
 * configure the UART at:
 * Asynchronous mode
 * double speed mode
 * eight byte character size
 * disabled parity bit
 * one stop bit
 * 9600 boud-rate
 *
 */
Uart_ConfigType UART_configuration = {ASYNCHRONOUS_DOUBLE_SPPED_MODE,EIGHT,DISABLED,ONE,9600};

/*
 * configure the timer at :
 * timer 1
 * compare mode
 * prescaler 1024
 * initial value=0
 * compare match value =250 to count 0.25 second every interrupt
 */

TIMER_ConfigType TIMER_configuration= {TIMER_0,COMPARE_MODE,F_CPU_1024,0,COMPARE_MATCH_VALUE};

int main(){
	UART_init(&UART_configuration); /* turn on the UART module to be able to communicate with HMI_ECU */
	SREG |= (1<<7); /* enable i- bit for interrupts */
	/*
	 * description of while one:
	 * the CONTROL_ECU is responsible of controlling the system
	 * so all what it do is waiting commands from HMI which take from the user
	 * every command points to specific action
	 */
	while (1){
		switch (receiveCommand()){
		/* when HMI sends PASSWORD_SEND, the CONTROL_ECU receives the
		 * password and store it in global variable called g_password*/
		case PASSWORD_SEND:
			receivePassword(g_password);
			break;
			/*
			 * if HMI sends PASSWORD_CONFIRMATION_SEND, the control_ECU takes the re-entered password
			 * and store it in another global variable called g_reenteredPassword and then
			 * compares the two input password if they are matched the CONTROL_ECU stores the password
			 * in the EEPROM and sends to HMI_ECU that the two password are matched by sending command
			 * called PASSWORD_MATCH
			 * if the two input password are not matched the CONTROL_ECU sends command to HMI_ECU
			 * to let it know that they are not matched *PASSWORD_MISMATCH*
			 * */
		case PASSWORD_CONFIRMATION_SEND:
			receivePassword(g_reenterdPassword);
			if (compareTwoPassword(g_password,g_reenterdPassword)==TRUE){
				savePassword(g_reenterdPassword);
				getSavedPassword(g_savedPassword);
				sendCommand(PASSWORD_MATCH);
			}
			else {
				sendCommand(PASSWORD_MISMATCH);

			}
			break;
			/*
			 * if HMI_ECU send CHECK_PASSWORD command, the CONTROL_ECU take the password and compare it
			 * to the password saved in EEPROM and reply with command which password correct or not
			 * if the CONTROL_ECU notices that the password has been entered wrong for three times
			 * it sends a command to HMI_ECU to let it know there is a thief
			 * */
		case CHECK_PASSWORD:
			receivePassword(g_password);
			getSavedPassword(g_savedPassword);
			if (compareTwoPassword(g_password,g_savedPassword)){
				sendCommand(PASSWORD_MATCH);
				g_wrong=0;
			}
			else {
				g_wrong++;
				if (g_wrong==MAX_WRONG_COUNTER){
					sendCommand(ERROR_MESSAGE);
					BUZZER_trigger();
					while (g_tick!=0){
						g_wrong=0;
					}
				}
				else {
					sendCommand(PASSWORD_MISMATCH);
				}
			}


			break;
			/*
			 * when the HMI_ECU sends OPEN_DOOR command the CONTROL_ECU turn on the motor to open
			 * the door and waits a some time then close the door
			 * */
		case OPEN_DOOR:
			openAndCloseDoor();
			break;
			/*
			 * when the HMI_ECU sends IF_SEAVED_COMMAND the CONTROL_ECU check if the password saved or
			 * not by comparing  the value in g_savedPassword to that exist in the EEPROM
			 * if it is equal to the default value of EEPROM it means that it is the first time to use
			 * the system and there is no entered password before and if it is not equal that means
			 * that there is password saved in the EEPROM
			 */


		case CHECK_IF_SAVED:

			switch(checkIfSaved()){
			case TRUE:
				sendCommand(SAVED);
				break;
			case FALSE:
				sendCommand(NOT_SAVED);
				break;
			}
			break;
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                   *
 *******************************************************************************/

/* Description :
 * function to send common command between two ECUs to the other ECU
 * every command sent indicate to specific action both ECUs know it
 * and making sure that both ECUs are ready to receive and send at specific time
 */
void sendCommand(uint8 command)
{

	UART_sendByte(READY);
	while(UART_receiveByte() != READY){};
	UART_sendByte(command);
	while(UART_receiveByte() != DONE){};

}
/* Description :
 * function to receive common command between two ECUs from the other ECU
 * every command sent indicate to specific action both ECUs know it
 * and making sure that both ECUs are ready to receive and send at specific time
 */
uint8 receiveCommand(void)
{

	while(UART_receiveByte() != READY){};
	UART_sendByte(READY);
	g_command=UART_receiveByte();
	UART_sendByte(DONE);
	return g_command;
}
/* description:
 * the ISR for TIMER0 to count a specific time for the motor to be rotated through
 */
void TIMER0_rotateMotorcount(void){

	g_tick++;
	if (g_tick % (NUM_OF_INTERRUPTS_PER_SECOND )== 0)
	{
		g_secondsCounter++;
	}
	if (g_secondsCounter == ( SECONDS_NUM_OPENING + SECONDS_NUM_CLOSING + SECONDS_NUM_HOLD )  )
	{
		g_tick=0;
		g_secondsCounter=0;
		TIMER_deinit(TIMER_0);
	}
}
/* description:
 * function which responsible of rotating the motor for specific time
 * first clock wise and hold the motor for some time then anti-clock wise
 * then stop the motor after the period is finished
 */
void openAndCloseDoor(void){
	DcMotor_Init();
	TIMER_init(&TIMER_configuration); /*turn on the timer to count  */
	TIMER_setCallBack(TIMER0_rotateMotorcount,TIMER_0); /* passing the ISR of TIMER0 */
	DcMotor_Rotate(CLOCKWISE);/* rotate the motor clock-wise for 15 seconds */
	while (g_secondsCounter != SECONDS_NUM_OPENING){
	}
	DcMotor_Rotate(STOP);/*hold the motor for 3-seconds */
	while (  g_secondsCounter != (SECONDS_NUM_OPENING + SECONDS_NUM_HOLD)){
	}
	DcMotor_Rotate(ANTI_CLOCKWISE); /*rotate the motor for 15-seconds */
	while ( g_secondsCounter != (SECONDS_NUM_OPENING + SECONDS_NUM_CLOSING+SECONDS_NUM_HOLD-1)){
	}
	DcMotor_Rotate(STOP); /* turn of the motor */
}
/* description:
 * the ISR for TIMER0 to count a specific time for the buzzer to be on within (60-seconds)
 */
void TIMER0_buzzer(void){
	g_tick++;
	/* counting the number of seconds according to number of interrupts*/
	if ((g_tick % NUM_OF_INTERRUPTS_PER_SECOND)==0){
		g_secondsCounter++;

	}
	if (g_secondsCounter==SECONDS_NUM_ERROR)
	{
		g_secondsCounter=0;
		g_tick=0;
		TIMER_deinit(TIMER_0);
	}


}
/* Description :
 * function to trigger the buzzer for 60-minutes and turn on the module for one time only using
 * static variable
 */
void BUZZER_trigger(void){
	static uint8 buzzerCounter=0; /* using static to turn on the module on time */
	if (buzzerCounter==0){
		BUZZER_init();
	}
	TIMER_init(&TIMER_configuration); /* turn on the timer to count 60-seconds */
	TIMER_setCallBack(TIMER0_buzzer,TIMER_0); /* passing the ISR of TIMER0 */
	BUZZER_on(); /*turn on the buzzer */
	while (g_secondsCounter != SECONDS_NUM_ERROR-1){/*waiting for 60-seconds */
	}
	BUZZER_off();/*turn off the buzzer after 60-seonds */
}
/* Description :
 * function to receive password from HMI_ECU but after making sure that both of them are
 * ready
 */
void receivePassword(uint8 *arr_password){
	uint8 i;
	while (UART_receiveByte() != (READY)){
	}
	UART_sendByte(READY);
	for(i=0;i<PASSWORD_SIZE;i++)
	{
		arr_password[i]=UART_receiveByte();
	}
	UART_sendByte(DONE);
}
/* Description :
 * function to check if two password are the same or not according to its size and digits
 */
uint8 compareTwoPassword(uint8 *arr_password1,uint8 *arr_password2){
	uint8 counter;
	for (counter=0;counter<PASSWORD_SIZE;counter++)
	{
		if (arr_password1[counter]!=arr_password2[counter]){
			return FALSE;
		}
	}
	return TRUE;
}
/* Description :
 * function to store the password in specific consecutive locations in EEPROM
 */
void savePassword(uint8 *arr_password){
	uint8 counter;
	EEPROM_init();
	for (counter=0;counter<PASSWORD_SIZE;counter++){
		EEPROM_writeByte(0x0311+counter,arr_password[counter]);
		_delay_ms(500);
	}
}
/* Description :
 * function to get the stored password from EEPROM and storing it in array of char
 * */
void getSavedPassword(uint8 *arr_password){
	uint8 counter;
	EEPROM_init();
	for (counter=0;counter<PASSWORD_SIZE;counter++){

		EEPROM_readByte(0x0311+counter,&arr_password[counter]);
		_delay_ms(500);
	}
}
/* Description :
 * function to check if the there is a stored password in EEPROM or it is the first
 *  for the system to be used
 */
uint8 checkIfSaved(void){
	uint8 counter;
	getSavedPassword(g_savedPassword); /*get the saved password from EEPROM */
	/* check if it is the default value of of EEPROM or another password is saved  */
	for( counter = 0 ; counter < PASSWORD_SIZE; counter++)
	{
		if(g_savedPassword[counter] != DEFUALT_VALUE_OF_EEPROM)
		{
			return TRUE; /*return true if it is not the same as the default */
		}
	}
	return FALSE; /* false in case it is the same as the default which means there is no
	               password entered before   */
}


