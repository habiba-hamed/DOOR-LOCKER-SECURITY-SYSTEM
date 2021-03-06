/*******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Description: Source file for BUZZER driver
 *
 * Created on: 28â€�/10â€�/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/
#include "buzzer.h"
#include "gpio.h"
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 *
 * function to configure the pin connected to the buzzer as output pin
 *
 */
void BUZZER_init  (void){
	/*configure the pin connected to the buzzer as output pin*/
	GPIO_setupPinDirection(PORT_ID,PIN_ID,PIN_OUTPUT);
	/* turn off the buzzer at the first */
	GPIO_writePin(PORT_ID, PIN_ID, LOGIC_LOW);
}
/*
 * Description :
 *
 * function to turn on the buzzer
 *
 */
void BUZZER_on    (void){
	/* turn on the buzzer by set the pin connected to it */
	GPIO_writePin(PORT_ID, PIN_ID, LOGIC_HIGH);

}
/*
 * Description :
 *
 * function to turn off the buzzer
 *
 */
void BUZZER_off   (void){
	/* turn off the buzzer by reset the pin connected to it */
		GPIO_writePin(PORT_ID, PIN_ID, LOGIC_LOW);

}
