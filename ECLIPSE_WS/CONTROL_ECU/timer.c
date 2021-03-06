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
static volatile void (*g_TIMER1_callBackPtr)(void) = NULL_PTR;
static volatile void (*g_TIMER2_callBackPtr)(void) = NULL_PTR;

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

/*ISRs for TIMER1 */
ISR(TIMER1_OVF_vect)
{
	if(g_TIMER1_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt  */
		(*g_TIMER1_callBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(g_TIMER1_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt  */
		(*g_TIMER1_callBackPtr)();
	}
}

/*ISRs for TIMER2 */
ISR(TIMER2_OVF_vect)
{
	if(g_TIMER2_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt  */
		(*g_TIMER2_callBackPtr)();
	}
}

ISR(TIMER2_COMP_vect)
{
	if(g_TIMER2_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the interrupt  */
		(*g_TIMER2_callBackPtr)();
	}
}
/*****************************************************************************************/



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
		} /* end of switch (Config_Ptr->mode) */
		break;
		case TIMER_1:
			/* clear all timer1 registers before accessing any of them */
			TIMER_deinit(Config_Ptr->timer_ID);
			/*WGM13=0 & WGM12=0 & WGM11 =0 & WGM10=0 at normal mode */
			/* set the initial value of timer1 anding with 0xFFFF to not exceeding 16-bits */
			TCNT1 =Config_Ptr->initialValue & 0xFFFF;
			/* Non PWM mode FOC1A=1 & FOC1B=1 */
			SET_BIT(TCCR1A,FOC1A);
			SET_BIT(TCCR1A,FOC1B);
			TCCR1B= (TCCR1B & (0xF8)) | Config_Ptr->prescaler; /* Set Timer1 clock */

			switch (Config_Ptr->mode){
			case OVERFLOW_MODE:
				/*
				 * Configure TIMER1 Timer/Counter1 Control Register A ?TCCR1A
				 * 1-COM1A0=0 & COM1A1=0 & COM1B0=0 & COM1B1=0 at normal mode
				 */
				SET_BIT(TIMSK,TOIE1); /*enable overflow interrupt */
				break;
			case COMPARE_MODE:
				OCR1A  = Config_Ptr->compareValue & 0xFFFF; /* set the compare value */
				SET_BIT(TCCR1B,WGM12); /* WGM13=0 & WGM12=1 & WGM11 =0 & WGM10=0 at CTC mode */
				SET_BIT(TIMSK,OCIE1A); /* enable compare interrupt for channel A */
				break;
			} /* end of switch (Config_Ptr->mode)  */
			break;
			case TIMER_2:
				/* clear registers of timer0 before accessing any of them */
				TIMER_deinit(Config_Ptr->timer_ID);
				/* set the initial value of timer1 anding with 0xFF to not exceeding 8-bits */
				TCNT2 =Config_Ptr->initialValue & 0xFF;
				TCCR2= (TCCR2 & (0xF8)) | Config_Ptr->prescaler; /* Set Timer2 initial clock */
				/* Non PWM mode FOC2=1 */
				SET_BIT(TCCR2,FOC2);
				/* COM21=0 & COM20=0 =>> OC0 is disconnected */
				switch(Config_Ptr->mode) {
				case OVERFLOW_MODE:

					/*WGM210=0 & WGM20=0 at normal mode */
					SET_BIT(TIMSK,TOIE2);  /* enable overflow interrupt */
					break;
				case COMPARE_MODE:
					/*WGM210=1 & WGM20=0 at CTC mode */
					SET_BIT(TCCR2,WGM21);
					OCR2  = Config_Ptr->compareValue & 0xFF; /* set the compare value */
					SET_BIT(TIMSK,OCIE2);/* enable compare interrupt  */
					break;
				} /*  end of switch(Config_Ptr->mode) */
				break;
	} /* end of switch (Config_Ptr->TIMER_ID) */
} /* end of TIMER_init function */

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
	case TIMER_1:
			g_TIMER1_callBackPtr = a_ptr;
			break;
	case TIMER_2:
			g_TIMER2_callBackPtr = a_ptr;
			break;
	}
} /* end of TIMER_setCallBack func */
/* Description :
 *
 * Function to turn off the module.
 *
 */
void TIMER_deinit(TIMER_ID timer_num){
	switch (timer_num){
	case TIMER_0:
		/* clear all timer0  */
		TCCR0 &= 0x00;
		TCNT0 &= 0x00;
		TIMSK &= 0x00;
		OCR0  &= 0x00;
		break;

	case TIMER_1:
		/* clear all timer1 registers  */
		TCCR1A &= 0x00;
		TCCR1B &= 0x00;
		TIMSK  &= 0x00;
		OCR1A  &= 0x00;
		TCNT1  &= 0x00;
		break;
	case TIMER_2:
		/* clear all timer2 registers  */
		TCCR2  &= 0x00;
		OCR2   &= 0x00;
		TIMSK  &= 0x00;
		TCNT2  &= 0x00;
		break;
	}
}
/* Description :
 *
 * Function to stop the timer.
 *
 */
void TIMER_STOP(TIMER_ID timer_num){
	switch (timer_num){
	case TIMER_0:
		TCCR0= (TCCR0 & (0xF8)); /* clear first 3-bits in TCCR0 register */
		break;
	case TIMER_1:
		TCCR1B= TCCR1B & (0xF8); /* clear first 3-bits in TCCR1B register */
			break;
	case TIMER_2:
		TCCR2= TCCR2 & (0xF8);   /* clear first 3-bits in TCCR2 register */
			break;

	}

}
