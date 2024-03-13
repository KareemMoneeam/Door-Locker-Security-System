 /******************************************************************************
 *
 * Module: Timer0 PWM
 *
 * File Name: Timer0_pwm.h
 *
 * Description: header file for the Timer0 PWM driver
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/
#ifndef TIMER0_PWM_H_
#define TIMER0_PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define TIMER0_OC0_PORT_ID			PORTB_ID
#define TIMER0_OC0_PIN_ID			PIN3_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void PWM_Timer0_Start(uint8 duty_cycle);
#endif /* TIMER0_PWM_H_ */
