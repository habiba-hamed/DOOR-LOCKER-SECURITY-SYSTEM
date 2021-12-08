/******************************************************************************
 *
 * Module: TIMER0
 *
 * File Name: timer0.h
 *
 * Description: Header file for TIMER0 driver
 *
 * Author: Habiba Hamed
 *
 *******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	OVERFLOW_MODE,COMPARE_MODE
}TIMER_Mode;
typedef enum{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}TIMER_Prescaler;
typedef enum{
	TIMER_0,TIMER_1,TIMER_2
}TIMER_ID;
typedef struct{
	TIMER_ID timer_ID;
	TIMER_Mode mode;
	TIMER_Prescaler prescaler;
	uint16 initialValue;
	uint16 compareValue;

}TIMER_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * function to initialize TIMER driver by:
 * 1-choose which timer to use
 * 2-setup the timer mode
 * 3-setup the timer prescaler
 * 4-setup the initial value of the timer
 * 5-setup the compare match value if the timer operating in the CTC mode
 */
void TIMER_init(const TIMER_ConfigType* Config_Ptr);
/* Description :
 *
 * Function to set the Call Back function address.
 *
 */
void TIMER_setCallBack(void(*a_ptr)(void),TIMER_ID timer_num);
/* Description :
 *
 * Function to turn off the module.
 *
 */
void TIMER_deinit(TIMER_ID timer_num);

/* Description :
 *
 * Function to stop the timer.
 *
 */
void TIMER_STOP(TIMER_ID timer_num);
#endif /* TIMER_H_ */
