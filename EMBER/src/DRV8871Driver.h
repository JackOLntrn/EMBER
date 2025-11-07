/** @file DRV8871Driver.h
 * @brief Driver class for the DRV8871 motor driver
 * @author Jackson Cordova, GPT-5 mini
 * @date November 2025
 * 
 * Lightweight, ESP32-friendly driver that uses the LEDC (PWM) API
 * for speed control and a GPIO for direction. The class is written
 * to be compatible with the Arduino core on ESP32 (uses ledc* APIs).
 */

#ifndef DRV8871_DRIVER_H
#define DRV8871_DRIVER_H
#include <Arduino.h>

class DRV8871Driver {
    protected:
        uint8_t in1Pin;      // PWM pin (attached to LEDC channel)
        uint8_t in2Pin;      // Direction pin (digital)
        uint8_t ledcChannel; // LEDC channel used for this instance
        uint8_t resolution;  // PWM resolution in bits (e.g., 8)

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

        // Coast (disable PWM) — motor freewheels
        void coastMotor(void);

        // Brake (active braking if supported by hardware)
        void brakeMotor(void);
};

#endif // DRV8871_DRIVER_H