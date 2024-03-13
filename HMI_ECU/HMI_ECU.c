/******************************************************************************
 *
 * Module: Human Machine Interface ECU - Application
 *
 * File Name: HMI_ECU.c
 *
 * Description: Source file for the HMI ECU application
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/

/*******************************************************************************
 *                            Necessary Files                                  *
 *******************************************************************************/
#include "HMI_ECU.h"
#include "avr/interrupt.h"
#include "util/delay.h"
#include "std_types.h"
#include "timer1.h"
#include "uart.h"
#include "keypad.h"
#include "lcd.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/* Password Arrays of defined size at HMI_ECU.h file */
static uint8 g_password1[PASSWORD_SIZE];
static uint8 g_password2[PASSWORD_SIZE];
/* Global variable to represent the current state within the HMI system sequence */
uint8 g_HMI_SYSTEM_SEQUENCE = CREATE_PASSWORD;
/* Global variable for Timer1 interrupts counter */
static uint8 g_tick = 0;
/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Private function to fill password arrays
 * its arguments are password size and the password array */
static void fillPasswordArray(uint8 size, uint8 *array) {
	/*Loop Counter*/
	uint8 i = 0;
	/*Pressed Key*/
	uint8 key = 0;
	/*Loop to fill password array */
	while (i < size) {
		/* Get the pressed key number,
		 * if any switch pressed for more than 500ms it will considered more than one press */
		key = KEYPAD_getPressedKey();
		if ((key >= 0) && (key <= 9)) {
			array[i] = key;
			LCD_displayCharacter('*');
			i++;
		}
		_delay_ms(500);
	}
	_delay_ms(100);
	LCD_clearScreen();
	LCD_displayString("= : Enter");
	/*wait using polling technique until the user press the Enter*/
	while (KEYPAD_getPressedKey() != ENTER) {
	}
	_delay_ms(500);
	LCD_clearScreen();
	return;
}

/*
 * Description :
 * Function to create the system password
 * */
void createPassword(void) {
	LCD_displayString("Create new");
	LCD_displayStringRowColumn(1, 0, "password");

	_delay_ms(1000);
	LCD_clearScreen();
	LCD_displayString("Plz enter pass:");
	LCD_moveCursor(1, 0);
	fillPasswordArray(PASSWORD_SIZE, g_password1);

	_delay_ms(500);
	LCD_clearScreen();
	LCD_displayString("Plz re-enter the");
	LCD_displayStringRowColumn(1, 0, "same pass: ");
	fillPasswordArray(PASSWORD_SIZE, g_password2);
	LCD_clearScreen();

}

/*
 * Description :
 * Function to send an array of bytes (a password) through UART to the Control ECU
 * */
void sendPassword(uint8 size, uint8 *password) {
	uint8 i;

	for (i = 0; i < size; i++) {
		UART_sendByte(password[i]);
		_delay_ms(3);
	}
}

/*
 * Description :
 * Private function to display the main options on an LCD
 * */
static void displayMainOptions(void) {
	LCD_displayString("+ : Open Door");
	LCD_displayStringRowColumn(1, 0, "- : Change Pass");
}

/*
 * Description :
 * Function to allow the user to choose between opening the door
 * or changing the password based on keypad input
 * */
void takeChoice(void) {
	uint8 choice = 0;
	/* Get the pressed key number,
	 * if any switch pressed for more than 250ms it will considered more than one press */
	while (choice != '+' && choice != '-') {
		choice = KEYPAD_getPressedKey();
	}
	if (choice == '+') {
		g_HMI_SYSTEM_SEQUENCE = OPEN_DOOR;
	} else if (choice == '-') {
		g_HMI_SYSTEM_SEQUENCE = CHANGE_PASSWORD;
	}
	UART_sendByte(g_HMI_SYSTEM_SEQUENCE);
}
/*
 * Description :
 * Function allows the user to input the created password
 * sends the password to the Control ECU to check it
 * */
void enterPassword(void) {
	uint8 userPassword[PASSWORD_SIZE];
	LCD_clearScreen();
	LCD_displayString("Enter your saved ");
	LCD_displayStringRowColumn(1,0,"password:  ");
	fillPasswordArray(PASSWORD_SIZE, userPassword);
	while (UART_receiveByte() != MC1_READY) {
	};
	sendPassword(PASSWORD_SIZE, userPassword);
}
/* Description :
 * Callback function for timer1 to control the LCD messages displaying time
 */
void controlMessageTime(void) {
	g_tick++;
}
/*
 * Description :
 * Function to display an error message for 1 Minute
 * */
void displayError(void) {
	/* Timer1 Configuration
	 * ---------------------
	 * F_Timer = 8MHz/1024(from Pre-scaler) = 7812 Hz
	 * T_Timer = 1/7812 = 128usec
	 * T_Compare = Compare Value * 128usec
	 * As I need 8 interrupts ( 8 compare matches ) per 60 second, so T_Compare = 60/8
	 * 60/8 = Compare Value * 128usec
	 * Compare Value = 7.5/128usec = 58594
	 * Compare Value = 58594
	 */
	Timer1_ConfigType TimerConfiguration =
	{ 0, 58594, PRESCALER_1024, CTC_MODE };
	Timer1_setCallBack(controlMessageTime);
	Timer1_init(&TimerConfiguration);
	while (g_tick < 8) {};
	g_tick = 0; /* Reset the interrupts counter */
	Timer1_deInit(); /* Reset timer1 registers */

	/*Go to Main Options again*/
	LCD_clearScreen();
}
/*******************************************************************************
 *                          MAIN FUNCTION                                      *
 *******************************************************************************/

int main(void) {
	/*Variable to count the number of consecutive failures of entering the password*/
	uint8 failuresCounter = 0;
	/* Enable Global Interrupt */
	SREG |= (1 << 7);
	/* LCD Initialization */
	LCD_init();

	/* Welcome Message */
	LCD_displayString("Door Locker");
	LCD_displayStringRowColumn(1, 0, "Security System");
	_delay_ms(1000);
	LCD_clearScreen();

	/* UART Configuration */
	UART_ConfigType UART_Configuration = { EIGHT_BITS_DATA, DISABLED,
			ONE_STOP_BIT, BAUD_RATE_9600_BPS };
	/* UART Initialization */
	UART_init(&UART_Configuration);

	while (1) {
		switch (g_HMI_SYSTEM_SEQUENCE) {

		case CREATE_PASSWORD:
			/* To create and verify a password by collecting two passwords from the user
			 * and sends them through UART to the Control ECU for comparison
			 * if they matched, the Control ECU will save it in the EEPROM
			 * if they not matched, the HMI ECU keep asking the user to create new password
			 * */
			do {
				createPassword();
				sendPassword(PASSWORD_SIZE, g_password1);
				sendPassword(PASSWORD_SIZE, g_password2);
			} while (UART_receiveByte() != PASSWORDS_MATCHED);
			LCD_displayString("PASSWORD SAVED!");
			_delay_ms(500);
			LCD_clearScreen();
			g_HMI_SYSTEM_SEQUENCE = MAIN_OPTIONS;
			break;
		case MAIN_OPTIONS:
			displayMainOptions();
			takeChoice(); /*That choice determines the next state of the system*/
			break;
		case OPEN_DOOR:
			do {
				if (failuresCounter < NUMBER_OF_CONSECUTIVE_FAILURES) {
					enterPassword();
					_delay_ms(10);
					failuresCounter++;
				} else {
					/* ERROR Message */
					LCD_displayString("ERROR!! YOU ARE");
					LCD_displayStringRowColumn(1, 0, "NOT AUTHORIZED");
					displayError();
					LCD_clearScreen();
					/*Return to the main options after error occurs*/
					g_HMI_SYSTEM_SEQUENCE = MAIN_OPTIONS;
				}
			} while (UART_receiveByte() != PASSWORDS_MATCHED);
			failuresCounter=0;
			/*Timer1 Configuration
			 * ---------------------
			 * F_Timer = 8MHz/1024(from Pre-scaler) = 7812 Hz
			 * T_Timer = 1/7812 = 128usec
			 * T_Compare = Compare Value * 128usec
			 * As I need two interrupts ( two compare matches ) per 15 second, so T_Compare = 15/2
			 * 15/2 = Compare Value * 128usec
			 * Compare Value = 7.5/128usec = 58594
			 * Compare Value = 58594
			 * */
			Timer1_ConfigType TimerConfiguration1 = { 0, 58594, PRESCALER_1024,
					CTC_MODE };
			Timer1_setCallBack(controlMessageTime);
			Timer1_init(&TimerConfiguration1);

			LCD_clearScreen();
			LCD_displayString("Door is");
			LCD_displayStringRowColumn(1, 0, "Unlocking..");

			while (g_tick < 2){};
			g_tick = 0; /*Reset the interrupts counter*/
			Timer1_deInit(); /* Reset timer1 registers*/
			/*For holding the door 3 seconds, we need to re-configure Timer1
			 * Compare Value = 23438*/

			Timer1_ConfigType TimerConfiguration2 = { 0, 23438, PRESCALER_1024,
					CTC_MODE };
			Timer1_setCallBack(controlMessageTime);
			Timer1_init(&TimerConfiguration2);

			LCD_clearScreen();
			LCD_displayString("Welcome Back!");

			while (g_tick < 1){};
			g_tick = 0; /*Reset the interrupts counter*/
			Timer1_deInit(); /*Reset timer1 registers*/

			/*We need to reconfigure timer1 to wait 15 seconds door locking time*/
			Timer1_ConfigType TimerConfiguration3 = { 0, 58594, PRESCALER_1024,
					CTC_MODE };
			Timer1_setCallBack(controlMessageTime);
			Timer1_init(&TimerConfiguration3);

			LCD_clearScreen();
			LCD_displayString("Door is");
			LCD_displayStringRowColumn(1, 0, "locking..");

			while (g_tick < 2) {};
			g_tick = 0; /*Reset the interrupts counter*/
			Timer1_deInit(); /*Reset timer1 registers*/
			LCD_clearScreen();
			g_HMI_SYSTEM_SEQUENCE = MAIN_OPTIONS;
			break;
		case CHANGE_PASSWORD:
			do {
				if (failuresCounter < NUMBER_OF_CONSECUTIVE_FAILURES) {
					enterPassword();
					_delay_ms(10);
					failuresCounter++;
				} else {
					/* ERROR Message */
					LCD_displayString("ERROR!! YOU ARE");
					LCD_displayStringRowColumn(1, 0, "NOT AUTHORIZED");
					displayError();
					LCD_clearScreen();
					/*Return to the main options after error occurs*/
					g_HMI_SYSTEM_SEQUENCE = MAIN_OPTIONS;
				}
			} while (UART_receiveByte() != PASSWORDS_MATCHED);
			failuresCounter=0;
			LCD_displayString("Change Password");
			_delay_ms(1000);
			LCD_clearScreen();
			/*REPEAT STEP 1*/
			g_HMI_SYSTEM_SEQUENCE = CREATE_PASSWORD;
			break;
		}
	}
}
