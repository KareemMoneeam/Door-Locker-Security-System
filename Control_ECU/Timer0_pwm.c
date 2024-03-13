 /******************************************************************************
 *
 * Module: Timer0 PWM
 *
 * File Name: Timer0_pwm.c
 *
 * Description: source file for the Timer0 PWM driver
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/

#include "Timer0_pwm.h"
#include "gpio.h"
#include "std_types.h"
#include "avr/io.h"


/*
 * Description :
 * Initialize the PWM module by:
 * 1. Trigger Timer0 with PWM Mode.
 * 2. Setup the PWM mode with Non-Inverting.
 * 3. Setup the prescaler with F_CPU/8
 * 4. Setup the compare value based on the required input duty cycle
 * 5. Setup the direction for OC0 as output pin
 */

void PWM_Timer0_Start(uint8 duty_cycle)
{

	TCNT0 = 0; // Set Timer Initial Value to 0

	OCR0 = (uint8)(((uint32)(duty_cycle*255)/100)); // Set Compare Value
	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */

	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);

	GPIO_setupPinDirection(TIMER0_OC0_PORT_ID,TIMER0_OC0_PIN_ID,PIN_OUTPUT);
}
