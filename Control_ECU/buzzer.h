/******************************************************************************
 *
 * Module: Buzzer
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define BUZZER_PORT_ID   		 PORTC_ID
#define BUZZER_PIN_ID  			 PIN2_ID
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Setup the direction for the buzzer pin as output pin through the GPIO driver.
 * Turn off the buzzer through the GPIO.
 */
void Buzzer_init();

/*
 * Description :
 * Function to enable the Buzzer through the GPIO.
 */
void Buzzer_on(void);


/*
 * Description :
 * Function to disable the Buzzer through the GPIO.
 */
void Buzzer_off(void);


#endif /* BUZZER_H_ */
