# Door Locker Security System

This project implements a Door Locker Security System using two ATmega32 Microcontrollers with a frequency of 8Mhz. It is designed based on a layered architecture model with the following components:

- **HMI_ECU (Human Machine Interface)**: Includes a 2x16 LCD and a 4x4 keypad for user interaction.
- **Control_ECU**: Manages the system's processing and decisions, including password checking, door unlocking, and system alarm activation. It includes EEPROM, a buzzer, and a DC motor.

## System Sequence

1. **Create a System Password**: The user sets a password consisting of 5 numbers, which is saved in the EEPROM.
2. **Main Options**: The LCD displays the main system options.
3. **Open Door**: User enters the password to unlock the door. If the password matches, the DC motor rotates to unlock the door.
4. **Change Password**: User can change the password by entering the current password and then entering a new one.
5. **Password Matching Handling**: If the password does not match, the system asks the user to re-enter it. If the password is not matched for the third consecutive time, the system activates the buzzer and locks the system for a minute.

## GPIO Driver

- Uses the same GPIO driver implemented in the course.
- Same driver used in both ECUs.

## LCD Driver

- Uses a 2x16 LCD.
- Uses the same LCD driver implemented in the course with 8-bits or 4-bits data mode.
- LCD is connected to the HMI_ECU.

## Keypad Driver

- Uses a 4x4 keypad.
- Keypad is connected to the HMI_ECU.

## DC Motor Driver

- Uses the same DC Motor driver implemented in the fan controller project.
- Motor is connected to the CONTROL_ECU.

## EEPROM Driver

- Uses the same external EEPROM driver controlled by I2C.
- EEPROM is connected to the CONTROL_ECU.

## I2C Driver

- Uses the same I2C driver implemented in the course.
- Used in the CONTROL_ECU to communicate with the external EEPROM.

## UART Driver

- Uses the same UART driver implemented in the course.
- Same driver used in both ECUs.

## Timer Driver

- Uses the same driver in both ECUs.
- Timer1 is used in the HMI_ECU for counting display message time and in the CONTROL_ECU for controlling the motor.

## Buzzer Driver

- Implements a full Buzzer driver.
- Buzzer is connected to the CONTROL_ECU.
