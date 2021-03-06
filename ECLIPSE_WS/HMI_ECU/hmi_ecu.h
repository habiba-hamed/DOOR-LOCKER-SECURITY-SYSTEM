/*******************************************************************************
 * module:   HMI_ECU
 *
 * File Name: hmi_ecu.h
 *
 * Description: Header file for Door locker security system application HMI_ECU
 *
 * Created on: 28‏/10‏/2021
 *
 * Author: Habiba Hamed
 *
 ********************************************************************************/

#ifndef HMI_ECU_H_
#define HMI_ECU_H_
/************************************************************************************************
 *                                      Definitions                                             *
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
#define DEFUALT_VALUE_OF_EEPROM                            1
#define SECONDS_NUM_OPENING                                15
#define SECONDS_NUM_CLOSING                                15
#define SECONDS_NUM_HOLD                                   3
#define SECONDS_NUM_ERROR                                  60
#define NUM_OF_INTERRUPTS_PER_SECOND                       4
#define COMPARE_MATCH_VALUE                                250


/************************************************************************************************
 *                                TYPED DECLARATION                                             *
 ************************************************************************************************/
typedef enum{
	PLUS='+',MINUS='-'
}mainOptionData;

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
/*
 * Description:
 * function to get the password from user and check if it valid or not and print ****
 * on the screen instead of the password and store it in global varibale
 */
void getPassword(void);
/*
 * description:
 * function to send the password to CONTROL_ECU
 * but at first check if CONTROL_ECU is ready to receive or not
 */
void sendPassword(uint8 *password_arr);
/*
 * description:
 * function to display enter new password screen
 */
void enterNewPassword(void);
/*
 * description:
 * function to display main options screen and get the user's choice
 * then pass it to another function called mainOptions
 */
void mainOptionsDisplay(void);
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
void mainOptions(mainOptionData data);
/*
 * description:
 * the ISR for TIMER0 to count a specific time for a message to be displayed on
 * screen during motor rotation
 */
void TIMER0_motorCount(void);
/*
 * description:
 * the ISR for TIMER0 to count a specific time for an error message to be displayed on
 * screen during a specific time
 */
void TIMER0_errorScreen(void);
/*
 * description:
 * function to display error message on the screen for specific time
 * and return to the main options screen after finishing the required time
 */
void ERROR_messageDisplay(void);
/*
 * description:
 * function to display messages on the screen to let user know
 * that the door is opening or closing according to the movement
 * of the motor and returning to the main options menu after finishing
 */
void doorOpeningAndClosing(void);

#endif /* HMI_ECU_H_ */
