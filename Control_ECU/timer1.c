/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.h
 *
 * Description: Source file for the Timer1 AVR driver
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/
#include "timer1.h"
#include <avr/io.h> /* To use Timer1 Registers */
#include <avr/interrupt.h> /* For Timer1 ISR */
#include "std_types.h"

/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

/* Global variables to hold the address of the call back function in the application */
static volatile void (*g_callBackPtr)(void) = NULL_PTR;

/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

/* Timer1 CTC Mode ISR */
ISR(TIMER1_COMPA_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/* Timer1 Normal Mode ISR */
ISR(TIMER1_OVF_vect)
{
	if(g_callBackPtr != NULL_PTR)
	{
		/* Call the Call Back function in the application after the edge is detected */
		(*g_callBackPtr)(); /* another method to call the function using pointer to function g_callBackPtr(); */
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/
/*
 * Description : Function to initialize the Timer driver
 *  1. Set the required mode.
 * 	2. Set the required clock.
 * 	3. Enable Timer1 CTC and Normal mode Interrupts.
 * 	4. Initialize Timer1 Registers
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{
	/* Initial Value for Timer1 */
	TCNT1 = Config_Ptr->initial_value;

	/* Set the required mode */
	/* Non-PWM Mode */
	TCCR1A |= (1<<FOC1A) | (1<<FOC1B);
	if (Config_Ptr->mode == NORMAL_MODE) {

		/*Enable Timer1 Normal Mode (WGM12=0, WGM13=0) and (WGM10=0, WGM11=0) by default in TCCR1A*/
		TCCR1B &= ~((Config_Ptr->mode) << 3);

		/*Timer1 Normal Mode Interrupt Enable*/
		TIMSK  |= (1 << TOIE1);
	}
	else if (Config_Ptr->mode == CTC_MODE) {

		/*Enable Timer1 CTC Mode (WGM12=1, WGM13=0) and (WGM10=0, WGM11=0) by default in TCCR1A
		 *And insert the required clock value in the first three bits (CS10, CS11 and CS12)
		 * of TCCR1B Register */
		TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->mode << 3) | (Config_Ptr->prescaler);

		/*Set the required Compare Match Value*/
		OCR1A  = Config_Ptr->compare_value;

		/*Timer1 CTC Mode Interrupt Enable*/
		TIMSK |= (1 << OCIE1A);
	}
}

/*
 * Description :
 * Function to disable Timer1
 */
void Timer1_deInit(void)
{
	/*Clear Timer1 Registers*/
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;
	OCR1A  = 0;

	/*Disable Timer1 Normal Mode and CTC Mode Interrupts*/
	TIMSK  &= ~(1 << TOIE1) & ~(1 << OCIE1A);
}
/*
 * Description :
 * Function to set the call back function address
 */
void Timer1_setCallBack(void(*a_ptr)(void))
{
	/* Save the address of the Call back function in a global variable */
	g_callBackPtr = a_ptr;
}
