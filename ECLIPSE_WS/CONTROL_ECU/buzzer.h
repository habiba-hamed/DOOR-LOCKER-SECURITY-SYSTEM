/*******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Description: Header file for BUZZER driver
 *
 * Created on: 28â€�/10â€�/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/*******************************************************************************
 *                              Definitions                                    *
 *******************************************************************************/
/* configure which pin is connected to the buzzer*/
#define PIN_ID  PIN3_ID
#define PORT_ID PORTC_ID

/*******************************************************************************
 *                      Functions prototypes                                  *
 *******************************************************************************/
/*
 * Description :
 *
 * function to configure the pin connected to the buzzer as output pin
 *
 */
void BUZZER_init  (void);
/*
 * Description :
 *
 * function to turn on the buzzer
 *
 */
void BUZZER_on    (void);
/*
 * Description :
 *
 * function to turn off the buzzer
 *
 */
void BUZZER_off   (void);


#endif /* BUZZER_H_ */
