/*******************************************************************************
 * module:   CONTROL_ECU
 *
 * File Name: control_ecu.h
 *
 * Description: Header file for Door locker security system application CONTROL_ECU
 *
 * Created on: 28‏/10‏/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/

#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_
/************************************************************************************************
 *                                        Definitions                                           *
 ************************************************************************************************/

#define READY                                              0x01
#define DONE                                               0x02
#define PASSWORD_SEND                                      0x03
#define PASSWORD_MATCH                                     0x04
#define PASSWORD_MISMATCH                                  0x05
#define PASSWORD_CONFIRMATION_SEND                         0x06
#define CHANGE_PASSWORD                                    0x07
#define OPEN_DOOR                                          0x08
#define CHECK_PASSWORD                                     0x09
#define ERROR_MESSAGE                                      0xA
#define CHECK_IF_SAVED                                     0xB
#define SAVED                                              0xC
#define NOT_SAVED                                          0xD
#define MAX_WRONG_COUNTER                                  3
#define PASSWORD_SIZE                                      5
#define DEFUALT_VALUE_OF_EEPROM                            0
#define SECONDS_NUM_OPENING                                15
#define SECONDS_NUM_CLOSING                                15
#define SECONDS_NUM_HOLD                                   3
#define SECONDS_NUM_ERROR                                  60
#define NUM_OF_INTERRUPTS_PER_SECOND                       4
#define COMPARE_MATCH_VALUE                                250

/************************************************************************************************
 *                                   FUNCTIONS Prototypes                                                 *
 ************************************************************************************************/
/* Description :
 * function to send common command between two ECUs to the other ECU
 * every command sent indicate to specific action both ECUs know it
 * and making sure that both ECUs are ready to receive and send at specific time
 */
void sendCommand(uint8 command);
/* Description :
 * function to receive common command between two ECUs from the other ECU
 * every command sent indicate to specific action both ECUs know it
 * and making sure that both ECUs are ready to receive and send at specific time
 */
uint8 receiveCommand(void);
/* description:
 * the ISR for TIMER0 to count a specific time for the motor to be rotated through
 */
void TIMER0_rotateMotorcount(void);
/* description:
 * function which responsible of rotating the motor for specific time
 * first clock wise and hold the motor for some time then anti-clock wise
 * then stop the motor after the period is finished
 */
void openAndCloseDoor(void);
/* description:
 * the ISR for TIMER0 to count a specific time for the buzzer to be on within (60-seconds)
 */
void TIMER0_buzzer(void);
/* Description :
 * function to trigger the buzzer for 60-minutes and turn on the module for one time only using
 * static variable
 */
void BUZZER_trigger(void);
/* Description :
 * function to receive password from HMI_ECU but after making sure that both of them are
 * ready
 */
void receivePassword(uint8 *arr_password);
/* Description :
 * function to check if two password are the same or not according to its size and digits
 */
uint8 compareTwoPassword(uint8 *arr_password1,uint8* arr_password2);
/* Description :
 * function to store the password in specific consecutive locations in EEPROM
 */
void savePassword(uint8 *arr_password);
/* Description :
 * function to get the stored password from EEPROM and storing it in array of char
 */
void getSavedPassword(uint8 *arr_password);
/* Description :
 * function to check if the there is a stored password in EEPROM or it is the first
 *  for the system to be used
 */
uint8 checkIfSaved(void);

#endif /* CONTROL_ECU_H_ */
