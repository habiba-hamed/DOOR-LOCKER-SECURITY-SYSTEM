/*******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.c
 *
 * Description: Source file for the DC Motor driver
 *
 * Created on: 12â€�/10â€�/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/
#include "GPIO.h"
#include "DC_motor.h"
#include <avr/io.h>



/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void DcMotor_Init(void){
	/*Configure the chosen 2 pins to connect the DC-Motor to as output pins */


	GPIO_setupPinDirection(DC_MOTOR_PORT1_ID,DC_MOTOR_PIN1_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_PORT2_ID,DC_MOTOR_PIN2_ID,PIN_OUTPUT);


	/* Stop at the DC-Motor at the beginning by clear the chosen 2 pins */


	GPIO_writePin(DC_MOTOR_PORT1_ID,DC_MOTOR_PIN1_ID,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_PORT2_ID,DC_MOTOR_PIN2_ID,LOGIC_LOW);

}
/*
 * Description :
 * 1. Rotate  or Stop the motor according to the state input variable.
 * 2. Control the motor speed 0 --> 100% from its maximum speed by sending to PWM driver.
 */
void DcMotor_Rotate(DcMotor_State state){
	/* configure the rotation of motor*/
	if(state==STOP){
		/* Stop the Motor */
		GPIO_writePin(DC_MOTOR_PORT1_ID,DC_MOTOR_PIN1_ID,LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_PORT2_ID,DC_MOTOR_PIN2_ID,LOGIC_LOW);

	}
	else if (state==CLOCKWISE){

		/* Rotates the Motor CW */
		GPIO_writePin(DC_MOTOR_PORT1_ID,DC_MOTOR_PIN1_ID,LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_PORT2_ID,DC_MOTOR_PIN2_ID,LOGIC_LOW);

	}
	else if (state == ANTI_CLOCKWISE){
		/* Rotates the Motor Anti clockwise */
				GPIO_writePin(DC_MOTOR_PORT1_ID,DC_MOTOR_PIN1_ID,LOGIC_LOW);
				GPIO_writePin(DC_MOTOR_PORT2_ID,DC_MOTOR_PIN2_ID,LOGIC_HIGH);

	}
}



