/*******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.h
 *
 * Description: Header file for the DC Motor Driver
 *
 * Created on: 12â€�/10â€�/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_
#include "std_Types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
/* The Ports and Pins IDs for the DC-Motor two pins */
#define DC_MOTOR_PORT1_ID   PORTC_ID
#define DC_MOTOR_PORT2_ID   PORTC_ID

#define DC_MOTOR_PIN1_ID    PIN6_ID
#define DC_MOTOR_PIN2_ID    PIN7_ID

/*******************************************************************************
 *                         Types Declaration                                  *
 *******************************************************************************/
typedef enum{
	STOP,CLOCKWISE,ANTI_CLOCKWISE
}DcMotor_State;


/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description : Function to initialize the DC Motor driver by:
 * 	1. setup the direction for the two motor pins.
 * 	2. Stop at the DC-Motor at the beginning.
 */

void DcMotor_Init(void);

/*
 * Description :
 * 1. Rotate  or Stop the motor according to the state input variable.
 * 2. Control the motor speed 0 --> 100% from its maximum speed by sending to PWM driver.
 */
void DcMotor_Rotate(DcMotor_State state);

#endif /* DC_MOTOR_H_ */
