

#include "DRV8871Driver.h"

DRV8871Driver::DRV8871Driver(uint8_t in1, uint8_t in2, uint8_t channel, uint32_t freq, uint8_t res)
{
    in1Pin = in1;
    in2Pin = in2;
    ledcChannel = channel;
    resolution = res;

    // Configure PWM channel and attach pin
    ledcSetup(ledcChannel, freq, resolution);
    ledcAttachPin(in1Pin, ledcChannel);

    // Ensure direction pin is an output
    pinMode(in2Pin, OUTPUT);
    pinMode(in1Pin, OUTPUT);
}

void DRV8871Driver::driveMotor(uint16_t speed, bool dir)
{
    ledcWrite(ledcChannel, speed);
    digitalWrite(in2Pin, dir ? HIGH : LOW);
}

void DRV8871Driver::coastMotor(void)
{
    // Stop PWM output (0 duty) and let motor coast
    ledcWrite(ledcChannel, 0);
    digitalWrite(in2Pin, LOW);
}

void DRV8871Driver::brakeMotor(void)
{
    // Set maximum duty and direction to apply braking depending on hardware
    uint16_t maxDuty = (1 << resolution) - 1;
    ledcWrite(ledcChannel, (uint32_t)maxDuty);
    digitalWrite(in2Pin, HIGH);
}