/** 
 * @title E.M.B.E.R - main file
 * @author Jackson Cordova, Matthieu Gol, GPT-5 mini
 * @date November 2025
 * @description: ...
 */
#include <Arduino.h>
#include "DRV8871Driver.h"
#include <printStream.h>


void setup() {
  Serial.begin(115200);
  DRV8871Driver Motor1(16,17,0,10000,8);
  Motor1.brakeMotor();
  delay(1000);
  Motor1.driveMotor(128, true);
  delay(1000);
  Motor1.brakeMotor();
  delay(200);
  for (uint8_t speed = 0; speed <= 255; speed += 5) {
    Motor1.driveMotor(speed, true);
    delay(100);
  }
  Motor1.coastMotor();
}

void loop() {
  Serial << "." << endl;
}