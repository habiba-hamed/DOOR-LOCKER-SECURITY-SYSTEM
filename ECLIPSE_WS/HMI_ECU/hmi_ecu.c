/*******************************************************************************
 * module:   HMI_ECU
 *
 * File Name: hmi_ecu.c
 *
 * Description: Source file for Door locker security system application HMI_ECU
 *
 * Created on: 28‏/10‏/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/

#include "keypad.h" /* to take the input from the user at the form of numbers */
#include "lcd.h"    /* to display the output and communicate with the user in friendly way */
#include "uart.h"   /* to communicate with CONTROL_ECU */
#include "timer.h"  /* to count time */
#include "util/delay.h"
#include "hmi_ecu.h"
#include  <avr/io.h>
#include "std_types.h"
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

/************************************************************************************************
 *                                GLOBAL VARIABLES                                              *
 ************************************************************************************************/

/*global array to store the first password entered in it */
uint8 g_password[PASSWORD_SIZE];
/*global ticks to count timer interrupts and
 been volatile to be able to used in ISR code */
volatile uint16 g_tick=0;
/*global variable to store the commands in it */
volatile uint8 g_command;
/*global ticks to count seconds counted by timer
 and been volatile to be able to used in ISR code */
uint8 g_secondsCounter=0;

int main()
{
	/* turn on the LCD module to display on it  */
	LCD_init();
	/* turn on the UART module to be able to communicate with CONTROL_ECU */
	UART_init(&UART_configuration);
	/* display welcome message on the screen when the user turn on the system */
	LCD_displayStringRowColumn(0,2,"DOOR LOCKER SECURITY SYSTEM");
	LCD_displayStringRowColumn(1,12,"WELCOME");
	/*_delay_ms(4000);*/

	SREG |= (1<<7);       /* Enable I-Bit for Interrupts */
	/*
	 * Description of while one:
	 * the HMI_ECU  is responsible of taking data from user and display messages on screen
	 * check if there is  password saved before in the EEPROM or not by asking CONTROL_ECU
	 * to know if it is the first time to use the system so make the user able to setup
	 * his password or there is password already at the EEPROM
	 * first time ==> setup new password
	 * not first time ==> display the main menu
	 */
	while (1){
		sendCommand(CHECK_IF_SAVED);
		switch(receiveCommand()){
		case SAVED:
			mainOptionsDisplay();
			break;
		case NOT_SAVED:
			enterNewPassword();
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

/*
 * Description:
 * function to get the password from user and check if it valid or not and print ****
 * on the screen instead of the password and store it in global variable
 */
void getPassword(void){
	uint8 counter=0,key;
	key=KEYPAD_getPressedKey();
	while(key != '=')
	{
		key=KEYPAD_getPressedKey();
		if( (key != '+' &&key != '-' &&key != '*' && key != '%' && key != '=') || (key>= 0 && key<= 9) )
		{
			if(counter < PASSWORD_SIZE)
			{
				g_password[counter]=key;
				LCD_displayCharacter('*');
				counter++;
			}
		}
		_delay_ms(1500);
	}
}

/*
 * description:
 * function to send the password to CONTROL_ECU
 * but at first check if CONTROL_ECU is ready to receive or not
 */
void sendPassword(uint8 *password_arr)
{
	UART_sendByte(READY);
	while(UART_receiveByte() != READY){};
	/* as soon as  CONTROL_ECU is ready HMI_ECU starts to send the password */
	for(uint8 i=0;i<PASSWORD_SIZE;i++)
	{
		UART_sendByte(password_arr[i]);
	}
	while(UART_receiveByte() != DONE){};
}


/*
 * description:
 * function to display enter new password screen
 */
void enterNewPassword(void){
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,"PLEASE ENTER NEW PASSWORD(5): ");
	LCD_moveCursor(1,13);
	getPassword(); /* take password from user and store it in global variable */
	/*send command to CONTROL_ECU to make sure to let it know
     that it is sending the password  */
	sendCommand(PASSWORD_SEND);
	sendPassword(g_password); /* sending the password to CONTROL_ECU */
	LCD_clearScreen();
	/* ask the user to Re-enter the password */
	LCD_displayStringRowColumn(0,0,"PLEASE RE-ENTER PASSWORD(5): ");
	LCD_moveCursor(1,13);
	getPassword(); /* take the password from the user and store it in the same global variable*/
	/*send command to CONTROL_ECU to ask it
     to check if two passwords are the same or not */
	sendCommand(PASSWORD_CONFIRMATION_SEND);
	/* sending the Re-entered password to CONTROL_ECU to get the confirmation*/
	sendPassword(g_password);
	/* check which command CONTROL_ECU has sent
	 * if it is PASSWORD_MATCH is means that it checked on the two password
	 * and they are matched and password has been changed so display message
	 * on the screen that NEW PASSWORD IS SET
	 * if the command is PASSWORD_MISMATCHED it means that two passwords are not matched
	 * and ask the user to enter new password again until entering two matched passwords */
	if(receiveCommand()==PASSWORD_MATCH){
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,6,"NEW PASSWORD IS SET");
		_delay_ms(4000);
		mainOptionsDisplay();
	}
	else{
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,8,"MISMATCHING!!");
		_delay_ms(4000);
		enterNewPassword();
	}
}
/*
 * description:
 * function to display main options screen and get the user's choice
 * then pass it to another function called mainOptions
 */
void mainOptionsDisplay(void){
	mainOptionData data;
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,7,"+: OPEN DOOR ");
	LCD_displayStringRowColumn(1,7,"-: CHANGE PAWSSWORD ");
	data =KEYPAD_getPressedKey() ;
	mainOptions(data);
}
/*
 * description:
 * function to ask the user to enter the password first then
 * if the password is correct it make him able to open the door
 * or change the password
 * and if the password is not correct, it gives the user the chance to
 * try 2 - times again
 * if he entered the password wrong 3- times
 * it display an error message for 60-seconds then return to the main options screen
 */
void mainOptions(mainOptionData data){
	LCD_clearScreen();
	/* ask the user to enter the password */
	LCD_displayStringRowColumn(0,0,"PLEASE ENTER THE PASSWORD(5): ");
	LCD_moveCursor(1,13);
	_delay_ms(3000);
	/*get the password from the user and send it to CONTROL_ECU to
	 * check if the entered password is correct or not */
	getPassword();
	sendCommand(CHECK_PASSWORD);
	sendPassword(g_password);
	/*
	 * 1- if the CONTROL_ECU replies with PASSWORD_MATCH it display message on screen
	 * that indicate that the door is opening in case of open the door
	 * or back to enter new password screen in case of change password
	 * 2- if the CONTROL_ECU replies with PASSWORD_MISMATCH it return to enter password
	 * screen after displaying a message which indicate that the entered password is wrong
	 * 3- if the CONTROL_ECU replies with ERROR_MESSAGE that indicate that the user
	 * entered the password wrong three times so it displaying an error message for 60-seconds
	 * then return to the main options menu
	 * */
	switch (receiveCommand()){
	case PASSWORD_MATCH:
		switch (data){
		case PLUS :
			sendCommand(OPEN_DOOR);
			doorOpeningAndClosing();
			break;
		case MINUS:
			enterNewPassword();
			break;
		}
		break;
		case PASSWORD_MISMATCH:
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,8,"WRONG PASSWORD!!!");
			_delay_ms(4000);
			mainOptions(data);
			break;
		case ERROR_MESSAGE:
			ERROR_messageDisplay();
			break;
	}
}
/*
 * description:
 * the ISR for TIMER0 to count a specific time for a message to be displayed on
 * screen during motor rotation
 */
void TIMER0_motorCount(void){
	g_tick++;
	/* counting number of seconds according to number of interrupts */
	if (g_tick % (NUM_OF_INTERRUPTS_PER_SECOND )== 0)
	{
		g_secondsCounter++;
	}

	if (g_secondsCounter == ( SECONDS_NUM_OPENING + SECONDS_NUM_CLOSING + SECONDS_NUM_HOLD )  )
	{
		/* turn off the timer after counting a specific time*/
		g_tick=0;
		g_secondsCounter=0;
		TIMER_deinit(TIMER_0);
	}
}
/*
 * description:
 * function to display messages on the screen to let user know
 * that the door is opening or closing according to the movement
 * of the motor and returning to the main options menu after finishing
 */
void doorOpeningAndClosing(void){
	LCD_clearScreen();
	TIMER_init(&TIMER_configuration); /*turn on the timer to count */
	TIMER_setCallBack(TIMER0_motorCount,TIMER_0); /* passing the ISR of timer0  */
	/* display a message on the screen to let the user know that the door is opening */
	LCD_displayStringRowColumn(0,8,"DOOR IS OPENING");
	while (g_secondsCounter!=SECONDS_NUM_OPENING){

	}LCD_clearScreen();
	while ( g_secondsCounter != (SECONDS_NUM_OPENING + SECONDS_NUM_HOLD)){
	}
	/* display a message on the screen to let the user know that the door is closing */
	LCD_displayStringRowColumn(0,8,"DOOR IS CLOSING");
	while ( g_secondsCounter != (SECONDS_NUM_OPENING + SECONDS_NUM_CLOSING+SECONDS_NUM_HOLD-1)){
	}
	mainOptionsDisplay();
}
/*
 * description:
 * the ISR for TIMER0 to count a specific time for an error message to be displayed on
 * screen during a specific time
 */
void TIMER0_errorScreen(void){
	g_tick++;
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
/*
 * description:
 * function to display error message on the screen for specific time
 * and return to the main options screen after finishing the required time
 */
void ERROR_messageDisplay(void){
	TIMER_init(&TIMER_configuration);
	TIMER_setCallBack(TIMER0_errorScreen,TIMER_0); /* passing the ISR of TIMER0 */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,12,"THIEF!!!");
	while (g_secondsCounter != (SECONDS_NUM_ERROR-1)){

	}
	mainOptionsDisplay();/*returning to the main options screen */
}



