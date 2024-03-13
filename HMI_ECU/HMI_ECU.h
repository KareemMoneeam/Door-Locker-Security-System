/******************************************************************************
 *
 * Module: Human Machine Interface ECU - Application
 *
 * File Name: HMI_ECU.h
 *
 * Description: Header file for the HMI ECU application
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/

#ifndef HMI_ECU_H_
#define HMI_ECU_H_

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define PASSWORD_SIZE 	     	5
#define ENTER			  	   '='

/* Passwords Checking */
#define PASSWORDS_UNMATCHED			   	 0
#define PASSWORDS_MATCHED				 1
#define NUMBER_OF_CONSECUTIVE_FAILURES   3

/* Timer1 Waiting Times */
#define HOLD_DOOR
/*Human Machine Interface System Sequence*/
#define CREATE_PASSWORD		    2
#define MAIN_OPTIONS			3
#define OPEN_DOOR				4
#define CHANGE_PASSWORD			5

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function to create the system password
 * */
void createPassword(void);

/*
 * Description :
 * Function to send the system password to the Control ECU through the UART
 * */
/*
 * Description :
 * Function to allow the user to choose between opening the door
 * or changing the password based on keypad input
 * */
void takeChoice(void);

/*
 * Description :
 * Function to allow the user to input the created password
 * sends the password to the Control ECU to check it
 * */
void enterPassword(void);

/*
 * Description :
 * Callback function for timer1 to control the LCD messages displaying time
 * */
void controlMessageTime(void);

/*
 * Description :
 * Function to display an error message for 1 Minute
 * */
void displayError(void);


#endif /* HMI_ECU_H_ */
