/******************************************************************************
 *
 * Module: Timer1
 *
 * File Name: timer1.h
 *
 * Description: Header file for the Timer1 AVR driver
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum{
	NO_CLOCK_SOURCE, NO_PRESCALING, PRESCALER_8, PRESCALER_64, PRESCALER_256, PRESCALER_1024,
	EXTERNAL_CLOCK_ON_FALLING_EDGE, EXTERNAL_CLOCK_ON_RISING_EDGE
}Timer1_Prescaler;

typedef enum{
	NORMAL_MODE, CTC_MODE=4
}Timer1_Mode;

typedef struct {
	uint16 initial_value;
	uint16 compare_value; // it will be used in compare mode only.
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;
}Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function to initialize the Timer driver
 */
void Timer1_init(const Timer1_ConfigType * Config_Ptr);

/*
 * Description :
 * Function to disable Timer1
 */
void Timer1_deInit(void);

/*
 * Description :
 * Function to set the call back function address
 */
void Timer1_setCallBack(void(*a_ptr)(void));


#endif /* TIMER1_H_ */
