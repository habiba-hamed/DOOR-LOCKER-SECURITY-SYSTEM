/******************************************************************************
 *
 * Module: TIMER0
 *
 * File Name: timer0.c
 *
 * Description: Source file for TIMER0 driver
 *
 * Author: Habiba Hamed
 *
 *******************************************************************************/
#include "avr/io.h"
#include "avr/interrupt.h"
#include "common_macros.h"
#include "timer.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_TIMER0_callBackPtr)(void) = NULL_PTR;
/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

/*ISRs for TIMER0 */
ISR(TIMER0_OVF_vect)
{
	if(g_TIMER0_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt  */
		(*g_TIMER0_callBackPtr)();
	}
}
ISR(TIMER0_COMP_vect)
{
	if(g_TIMER0_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt  */
		(*g_TIMER0_callBackPtr)();
	}
}
/*******************************************************************************
 *                      Functions Definitions                                   *
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
void TIMER_init(const TIMER_ConfigType* Config_Ptr){
	switch (Config_Ptr->timer_ID){
	case TIMER_0 :
		/* clear registers of timer0 before accessing any of them */
		TIMER_deinit(Config_Ptr->timer_ID);
		TCNT0 = Config_Ptr->initialValue;
		/* Configure timer0 control register
		 * 1. Non PWM mode FOC0=1
		 * 2. setup the prescaler
		 */
		SET_BIT(TCCR0,FOC0);
		TCCR0= (TCCR0 & (0xF8)) | Config_Ptr->prescaler; /* set the clock */
		switch (Config_Ptr->mode){
		case OVERFLOW_MODE:
			SET_BIT(TIMSK,TOIE0); /* Enable Timer0 Overflow Interrupt*/
			break;
		case COMPARE_MODE:
			OCR0  = Config_Ptr->compareValue & 0xFF ; /* Set Compare Value
			anding with 0xFF to not exceeding 8-bits */
			SET_BIT(TIMSK,OCIE0); /* Enable Timer0 Compare Interrupt */
			/* Configure timer0 control register for compare mode
			 * 1- CTC Mode WGM01=1 & WGM00=0
			 * 2- OCO is disconnected
			 */
			SET_BIT(TIMSK,WGM01);
			break;
		}
	}
}
/* Description :
 *
 * Function to set the Call Back function address.
 *
 */
void TIMER_setCallBack(void(*a_ptr)(void),TIMER_ID timer_num){
	/* Save the address of the Call back function in a global variable */
	switch (timer_num){
	case TIMER_0:
		g_TIMER0_callBackPtr = a_ptr;
		break;
	}
}

void TIMER_deinit(TIMER_ID timer_num){
	switch (timer_num){
	case TIMER_0:
		/* clear all timer0  */
		TCCR0 &= 0x00;
		TCNT0 &= 0x00;
		TIMSK &= 0x00;
		OCR0  &= 0x00;
		break;
	}
}

