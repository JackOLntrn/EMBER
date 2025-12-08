/** @file DRV8871Driver.cpp
 * This file implements a class to provide basic control for 
 * a DRV8871 motor driver using PWM for speed control. 
 * To use this code you will need PWM capable output pins.
 * 
 * @brief Driver class for the DRV8871 motor driver
 * @author Jackson Cordova, GPT-5 mini
 * @date November 2025
 */

 #include <Arduino.h>


#ifndef DRV8871_DRIVER_H
#define DRV8871_DRIVER_H

/**
 * @class DRV8871Driver
 * @brief A class to add basic functions for DRV8871 Motor Driver
 *
 * 
 * @details This class enables drive, coast, and brake functions 
 * for a DRV8871 motor driver. It uses PWM for speed control and 
 * a digital pin for direction control. This class uses 8 bit
 * PWM resolution, so speed values should be between 0 and 255.
 */
class DRV8871Driver {
    protected:
        uint8_t in1Pin;      // PWM pin (attached to LEDC channel)
        uint8_t in2Pin;      // Direction pin (digital)
        
    public:
        /**
         * Constructor
         * @param in1 PWM output pin
         * @param in2 Direction pin
         */
        DRV8871Driver(uint8_t in1, uint8_t in2);

        /**
         * Drive the motor
         * @param speed unsigned speed value. Range: 0 to max based on resolution
         * @param dir   Direction (true/false) written to the direction pin
         */
        void driveMotor(uint8_t speed, bool dir);

        /**
         * Coast the Motor (disable PWM) - slowly come to a stop from friction
         */
        void coastMotor(void);

        /**
         * Brake the motor quickly - quickly stop the motor using active braking
         */
        void brakeMotor(void);
};

#endif // DRV8871_DRIVER_H