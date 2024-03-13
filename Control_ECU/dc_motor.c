/******************************************************************************
 *
 * Module: DC Motor
 *
 * File Name: dc_motor.h
 *
 * Description: source file for the DC Motor driver
 *
 * Author: Kareem Abd El-Moneam
 *
 *******************************************************************************/
#include "dc_motor.h"
#include "gpio.h"
#include "std_types.h"
#include "Timer0_pwm.h"

/*
 * Description :
 * The Function responsible for setup the direction for the two motor pins through the GPIO driver.
 * */
void DcMotor_init(void)
{
	GPIO_setupPinDirection(DC_MOTOR_IN1_PORT_ID, DC_MOTOR_IN1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_IN2_PORT_ID, DC_MOTOR_IN2_PIN_ID, PIN_OUTPUT);
	/* Stop the DC-Motor at the beginning through the GPIO driver */
	GPIO_writePin(DC_MOTOR_IN1_PORT_ID, DC_MOTOR_IN1_PIN_ID, LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_IN2_PORT_ID, DC_MOTOR_IN2_PIN_ID, LOGIC_LOW);
}

/*
 * Description :
 * The function responsible for rotate the DC Motor CW/ or A-CW or stop
 * the motor based on the state input state value.
 * Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DcMotor_Rotate(DcMotor_State state, uint8 speed)
{
	switch(state)
	{
	case STOP:
		/* Stop DC Motor */
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID, DC_MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID, DC_MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	case cw:
		/* Clockwise Rotation */
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID, DC_MOTOR_IN1_PIN_ID, LOGIC_LOW);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID, DC_MOTOR_IN2_PIN_ID, LOGIC_HIGH);
		break;
	case ACW:
		/* Stop DC Motor */
		GPIO_writePin(DC_MOTOR_IN1_PORT_ID, DC_MOTOR_IN1_PIN_ID, LOGIC_HIGH);
		GPIO_writePin(DC_MOTOR_IN2_PORT_ID, DC_MOTOR_IN2_PIN_ID, LOGIC_LOW);
		break;
	}
	/* Send the required duty cycle to the PWM driver based on the required speed value */
	PWM_Timer0_Start(speed);

}
