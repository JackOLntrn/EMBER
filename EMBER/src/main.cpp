/** 
 * @title E.M.B.E.R - main file
 * @author Jackson Cordova, Matthieu Gol, GPT-5 mini
 * @date November 2025
 * @description: ...
 */
#include <Arduino.h>
#include "DRV8871Driver.h"


void setup() {
  Serial.begin(115200);
  // make the motor driver instance static so it persists after setup()
  static DRV8871Driver Motor1(16,17);
  Motor1.brakeMotor();
  delay(1000);
  Serial.println("Driving motor forward at half speed");
  Motor1.driveMotor(128, true);
  delay(1000);
  Serial.println("Braking motor");
  Motor1.brakeMotor();
  delay(2000);
  Serial.println("Ramping motor speed up");
  // use an int here to avoid 8-bit wrap-around which would make the loop infinite
  for (int speed = 0; speed <= 255; speed += 5) {
    Motor1.driveMotor(speed, true);
    delay(100);
  }
  Motor1.coastMotor();
  delay(1000);
  Motor1.driveMotor(255, true);
  delay(20);
  for (int speed = 255; speed >= 0; speed -= 5) {
    Motor1.driveMotor(speed, true);
    delay(100);
  }

  for (int speed = 0; speed <= 255; speed += 5) {
    Motor1.driveMotor(speed, true);
    delay(100);
  }

  
  
  Motor1.coastMotor();

  delay(2000);
  Motor1.driveMotor(200, false);
  delay(1000);
  Motor1.brakeMotor();
}

void loop() {
  Serial.println(".");
  delay(1000);
}