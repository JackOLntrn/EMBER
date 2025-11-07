

#include "DRV8871Driver.h"

DRV8871Driver::DRV8871Driver(uint8_t in1, uint8_t in2) 
{
    in1Pin = in1;
    in2Pin = in2;

    // Ensure direction pin is an output
    pinMode(in2Pin, OUTPUT);
    pinMode(in1Pin, OUTPUT);
}

void DRV8871Driver::driveMotor(uint8_t speed, bool dir)
{
    if (dir) {
        speed = 255 - speed;
    }

    analogWrite(in1Pin, speed);
    digitalWrite(in2Pin, dir ? HIGH : LOW);
}

void DRV8871Driver::coastMotor(void)
{
    // Stop PWM output (0 duty) and let motor coast
    analogWrite(in1Pin, 0);
    digitalWrite(in2Pin, LOW);
}

void DRV8871Driver::brakeMotor(void)
{
    // Set maximum duty and direction to apply braking depending on hardware
    analogWrite(in1Pin, 255); // assuming 8-bit resolution
    digitalWrite(in2Pin, HIGH);
}