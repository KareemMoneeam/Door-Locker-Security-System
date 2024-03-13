/******************************************************************************
 *
 * Module: Control ECU - Application
 *
 * File Name: Control_ECU.h
 *
 * Description: Source file for the Control ECU application
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/

/*******************************************************************************
 *                            Necessary Files                                  *
 *******************************************************************************/
#include "Control_ECU.h"
#include "avr/interrupt.h"
#include "util/delay.h"
#include "std_types.h"
#include "timer1.h"
#include "uart.h"
#include "twi.h"
#include "external_eeprom.h"
#include "buzzer.h"
#include "dc_motor.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/
/*Global variable to store the password matching status*/
uint8 g_passwordFlag = PASSWORDS_UNMATCHED;
/*Password Arrays of defined size at Control_ECU.h file*/
uint8 g_receivedPassword1[PASSWORD_SIZE];
uint8 g_receivedPassword2[PASSWORD_SIZE];
/* Global variable to represent the current state within the system sequence */
uint8 g_CONTROL_SYSTEM_SEQUENCE = VERIFY_NEW_PASSWORD;
/* Global variable for Timer1 interrupts counter */
static uint8 g_tick = 0;

/*Global VIRTUAL EEPROM (Array) to check the logic before saving the passwords*/
//uint8 EEPROM[PASSWORD_SIZE];

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description :
 * Function to receive two passwords through UART
 * */
void receiveTwoPasswords(void) {
	uint8 i;

	/*Loop to receive the first password through UART */
	for (i = 0; i < PASSWORD_SIZE; i++) {
		g_receivedPassword1[i] = UART_receiveByte();
	}
	/*Loop to receive the second password through UART */
	for (i = 0; i < PASSWORD_SIZE; i++) {
		g_receivedPassword2[i] = UART_receiveByte();

	}
}

/* Description:
 * Function to confirm and compare two passwords received through UART
 */
void confirmPassword(void) {
	uint8 i;

	/*Loop to compare the two received passwords */
	for (i = 0; i < PASSWORD_SIZE; i++) {
		if (g_receivedPassword1[i] == g_receivedPassword2[i]) {
			g_passwordFlag = PASSWORDS_MATCHED;
		} else {
			g_passwordFlag = PASSWORDS_UNMATCHED;
			break;
		}
	}
	UART_sendByte(g_passwordFlag);
}
/*
 * Description :
 * Function to save a received password into EEPROM memory
 * The password is written to EEPROM memory at a specific address
 * Each element of the password array stored sequentially
 * */
void savePassword(void) {
	uint8 i;
	for (i = 0; i < PASSWORD_SIZE; i++) {
		//EEPROM[i] = g_receivedPassword1[i];
		EEPROM_writeByte(EEPROM_SAVE_ADDRESS + i, g_receivedPassword1[i]);
		_delay_ms(10);
	}
}
/*
 * Description :
 * Function to receive a password from another ECU using UART and
 * read the stored password from EEPROM in the Control ECU.
 *
 * Parameters:
 * - receivedPassword: Array to store the received password.
 * - storedPassword: Array to store the password read from EEPROM.
 */
void receive_read_Password(uint8 receivedPassword[PASSWORD_SIZE], uint8 storedPassword[PASSWORD_SIZE])
{
    uint8 i;

    UART_sendByte(MC1_READY);

    for (i = 0; i < PASSWORD_SIZE; i++) {
        receivedPassword[i] = UART_receiveByte();
    }

    for (i = 0; i < PASSWORD_SIZE; i++) {
        EEPROM_readByte(EEPROM_SAVE_ADDRESS + i, &storedPassword[i]);
    }
}
/*
 * Description :
 * - Function to compare between the input password at HMI ECU
 * and the stored password at EEPROM in Control ECU
 * - Call the alarm if the two passwords are not matched
 * for a number of consecutive times
 * */
void checkPassword(void) {
	uint8 i;
    uint8 receivedPassword[PASSWORD_SIZE];
    uint8 storedPassword[PASSWORD_SIZE];
    uint8 failuresCounter = 0;
    receive_read_Password(receivedPassword, storedPassword);

	/*Loop to check the number of failures during user inputs the password*/
	while (failuresCounter < NUMBER_OF_CONSECUTIVE_FAILURES) {
		boolean isFailure = 0; /* Assume no failure initially */
		for (i = 0; i < PASSWORD_SIZE; i++) {
			if (receivedPassword[i] != storedPassword[i]) {
				isFailure = 1;
				break; /* Break out of the loop when a failure occurs*/
			}
		}
		if (isFailure) {
			failuresCounter++;
		    if(failuresCounter == NUMBER_OF_CONSECUTIVE_FAILURES)
		    {
		    	break;
		    }
			g_passwordFlag = PASSWORDS_UNMATCHED;
			UART_sendByte(g_passwordFlag);

			/* fetch new password from HMI ECU*/
		    receive_read_Password(receivedPassword, storedPassword);
		} else {
			failuresCounter = 0; /* Reset consecutive failures count on success*/
			g_passwordFlag = PASSWORDS_MATCHED;
			break;
		}
	}
	if (failuresCounter < NUMBER_OF_CONSECUTIVE_FAILURES) {
		UART_sendByte(g_passwordFlag);
	}
	else {
		g_passwordFlag = PASSWORDS_UNMATCHED;
		UART_sendByte(g_passwordFlag);
		/* ACTIVATE BUZZER (ALARM) FOR 1 MINUTE */

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
		Timer1_ConfigType TimerConfiguration = { 0, 58594, PRESCALER_1024,
				CTC_MODE };
		Timer1_init(&TimerConfiguration);
		Buzzer_on();
		Timer1_setCallBack(activateAlarm);
	}
}

/*
 * Description :
 * Callback Function to activate an alarm using Timer1 and Buzzer with for 1 Minute
 * */
void activateAlarm(void) {
	g_tick++;
	if (g_tick == 8) {
		Buzzer_off();
		g_tick = 0; /* Reset the interrupts counter */
		Timer1_deInit(); /* Reset timer1 registers */
	}
}
/*
 * Description :
 * Callback function for timer1, controlling the door using DC-Motor
 * */
void doorControl(void) {

	g_tick++;
	/* TWO Interrupts for 15 Second*/
	if (g_tick == 2) {
		Timer1_deInit(); /* Reset timer1 registers */

		/* For holding the door 3 seconds, we need to re-configure Timer1
		 * Compare Value = 23438 */
		Timer1_ConfigType TimerConfiguration2 = { 0, 23438, PRESCALER_1024,
				CTC_MODE };
		Timer1_init(&TimerConfiguration2);
		DcMotor_Rotate(STOP, 0);
	}
	/* One Interrupt for 3 Seconds with the new configuration*/
	if (g_tick == 3) {

		Timer1_deInit(); /* Reset timer1 registers */

		/*We need to reconfigure timer1 to wait 15 seconds door locking time*/
		Timer1_ConfigType TimerConfiguration3 = { 0, 58594, PRESCALER_1024,
				CTC_MODE };
		Timer1_init(&TimerConfiguration3);
		DcMotor_Rotate(ACW, 100);
	}
/* TWO Interrupts for 15 Second with the previous configuration*/
	if (g_tick == 5) {
		g_tick = 0; /* Reset the interrupts counter */
		Timer1_deInit(); /* Reset timer1 registers */
		DcMotor_Rotate(STOP, 0);
	}
}

/*******************************************************************************
 *                          MAIN FUNCTION                                      *
 *******************************************************************************/

int main(void) {
	/* Enable Global Interrupts */
	SREG |= (1 << 7);
	/* UART Configuration */
	UART_ConfigType UART_Configuration = { EIGHT_BITS_DATA, DISABLED,
			ONE_STOP_BIT, BAUD_RATE_9600_BPS };
	/* UART Initialization */
	UART_init(&UART_Configuration);

	/* I2C Configuration */
	TWI_ConfigType TWI_Configuration = { 0x01, FAST_RATE_MODE };
	/* I2C Initialization */
	TWI_init(&TWI_Configuration);

	/* Dc-Motor Initialization */
	DcMotor_init();

	/* Buzzer Initialization */
	Buzzer_init();

	while (1) {
		switch (g_CONTROL_SYSTEM_SEQUENCE) {
		case VERIFY_NEW_PASSWORD:
			/* Verify a new password by receiving it from the HMI ECU through UART.
			 * if the two passwords are matched the Control ECU will save it in the EEPROM
			 * if not, the Control ECU keeps receiving new passwords*/
			while (g_passwordFlag != PASSWORDS_MATCHED) {
				receiveTwoPasswords();
				_delay_ms(15);
				confirmPassword();
			}
			if (g_passwordFlag == PASSWORDS_MATCHED) {
				savePassword();
			}
			g_CONTROL_SYSTEM_SEQUENCE = MAIN_OPTIONS;
			g_passwordFlag = PASSWORDS_UNMATCHED; /*resets the flag*/
			break;
		case MAIN_OPTIONS:
			/*To know the next step for Control ECU we use the HMI_SYSTEM_SEQUENCE*/
			if (UART_receiveByte() == OPEN_DOOR) {
				g_CONTROL_SYSTEM_SEQUENCE = OPEN_DOOR;
			} else {
				g_CONTROL_SYSTEM_SEQUENCE = CHANGE_PASSWORD;
			}
			break;
		case OPEN_DOOR:
			checkPassword();
			if (g_passwordFlag == PASSWORDS_MATCHED) {

				/* Timer1 Configuration
				 * ---------------------
				 * F_Timer = 8MHz/1024(from Pre-scaler) = 7812 Hz
				 * T_Timer = 1/7812 = 128usec
				 * T_Compare = Compare Value * 128usec
				 * As I need two interrupts ( two compare matches ) per 15 second, so T_Compare = 15
				 * 15/2 = Compare Value * 128usec
				 * Compare Value = 7.5/128usec = 58594
				 * Compare Value = 58594
				 */
				Timer1_ConfigType TimerConfiguration1 = { 0, 58594,
						PRESCALER_1024, CTC_MODE };
				Timer1_init(&TimerConfiguration1);
				DcMotor_Rotate(cw, 100);
				Timer1_setCallBack(doorControl);
			}
			g_passwordFlag = PASSWORDS_UNMATCHED; /*reset the flag*/
			break;
		case CHANGE_PASSWORD:
			checkPassword();
			if (g_passwordFlag == PASSWORDS_MATCHED) {
				/*REPEAT STEP 1*/
				g_CONTROL_SYSTEM_SEQUENCE = VERIFY_NEW_PASSWORD;
			}
			g_passwordFlag = PASSWORDS_UNMATCHED; /*reset the flag*/
			break;
		}
	}
}

