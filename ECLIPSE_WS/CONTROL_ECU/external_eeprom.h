/******************************************************************************
 *
 * Module: External EPPROM
 *
 * File Name: external_eeprom.h
 *
 * Description: Header file for the External EEPROM driver
 *
 * Author: Habiba Hamed
 *
 *******************************************************************************/
#ifndef EXTERNAL_EEPROM_H_
#define EXTERNAL_EEPROM_H_
#include "std_types.h"
/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define SUCCESS 1
#define ERROR   0
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 *
 * function to initialize the I2C(TWI) module inside the MC
 *
 */
 void EEPROM_init(void);
 /*
   * Description :
   *
   * function to write byte inside specific memory location in EEPROM
   *
   */
 uint8 EEPROM_writeByte(uint16 u16address,uint8 u8data);
 /*
    * Description :
    *
    * function to read byte from specific memory location in EEPROM
    *
    */
 uint8 EEPROM_readByte(uint16 u16address, uint8*u8data);
#endif /* EXTERNAL_EEPROM_H_ */
