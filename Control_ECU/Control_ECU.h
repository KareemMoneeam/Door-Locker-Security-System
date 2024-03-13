/******************************************************************************
 *
 * Module: Control ECU - Application
 *
 * File Name: Control_ECU.h
 *
 * Description: Header file for the Control ECU application
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/
#ifndef CONTROL_ECU_H_
#define CONTROL_ECU_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PASSWORD_SIZE 			          5
#define PASSWORDS_UNMATCHED		          0
#define PASSWORDS_MATCHED			      1
#define EEPROM_SAVE_ADDRESS 			  0x0400 /* Saving address in EEPROM for storing data */
#define FIRST_TRIAL						  1
#define SECOND_TRIAL					  2
#define NUMBER_OF_CONSECUTIVE_FAILURES    3

/*Control System Sequence*/
#define VERIFY_NEW_PASSWORD		2
#define MAIN_OPTIONS			3
#define OPEN_DOOR				4
#define CHANGE_PASSWORD			5

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
/* Description:
 * Function to confirm and compare two passwords received through UART
 */
void confirmPassword(void);

/*
 * Description :
 * Function to receive two passwords through UART
 * */
void receiveTwoPasswords(void);

/* Description:
 * Function to save a received password into EEPROM memory
 * The password is written to EEPROM memory at a specific address
 * Each element of the password array stored sequentially
 * */
void savePassword(void);

/*
 * Description :
 * - Function to compare between the input password at HMI ECU
 * and the stored password at EEPROM in Control ECU
 * - Call the alarm if the two passwords are not matched
 * for a number of consecutive times
 * */
void checkPassword(void);

/*
 * Description :
 * Function to receive a password from another ECU using UART and
 * read the stored password from EEPROM in the Control ECU.
 *
 * Parameters:
 * - receivedPassword: Array to store the received password.
 * - storedPassword: Array to store the password read from EEPROM.
 */
void receive_read_Password(uint8 receivedPassword[PASSWORD_SIZE], uint8 storedPassword[PASSWORD_SIZE]);

/*
 * Description :
 * Callback function for timer1, controlling the door using DC-Motor
 * */
void doorControl(void);

/*
 * Description :
 * Callback Function to activate an alarm using Timer1 and Buzzer with for 1 Minute
 * */
void activateAlarm(void);
#endif /* CONTROL_ECU_H_ */
