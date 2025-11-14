/** @file task_motorControl.cpp
 * Task for controlling the pan and tilt motors
 * 
 * @author Jackson Cordova
 * @date November 2025
 */

#include "task_motorControl.h"
#include <Arduino.h>
#include "DRV8871Driver.h"
#include <ESP32Encoder.h>
#include "PrintStream.h"
#include "shares.h"

const float Kp1 = 5.0;
const float Ki1 = 0;
const float Kd1 = 0;

 
 /** @brief task which controls the pan and tilt motors
  * @details when a fire is not present, this task will drive 
  * the motors in a sinusoidal pattern. When a fire is present,
  * this task will drive the motors to a reference position
  * specified by the panRefCount and tiltRefCount shares.
  * 
  * @param p_params not used and should be set to void in the xTaskCreate call
  */
 void task_motorControl(void* p_params) {
    ESP32Encoder enc1; // create encoder object for motor 1
    ESP32Encoder enc2; // create encoder object for motor 2
    //initialize motors and encoders
    DRV8871Driver Motor1(16,17); 
    //DRV8871Driver Motor2(12,13);
    enc2.attachHalfQuad(34,35);
    enc1.attachHalfQuad(32,33);
    // ensure motors are stopped at startup
    Motor1.brakeMotor();
    //Motor2.brakeMotor();
    int32_t panErr, tiltErr;
    int8_t P1, D1, P2, D2;
    float I1, I2;
    P1 = 0;
    P2 = 0;
    I1 = 0;
    I2 = 0;
    int16_t panSpeed, tiltSpeed;
    while(true){
        if (fire.get()) {
            panErr = panRefCount.get() - enc1.getCount();
            tiltErr = tiltRefCount.get() - enc2.getCount();
        }
        else {
            panErr = sin(millis() / 5000) * 500 -  enc1.getCount();
            tiltErr = cos(millis() / 5000) * 500 - enc2.getCount();
        }

        D1 = (panErr - P1)*10;
        P1 = panErr;
        I1 = (I1 + panErr * 0.01);
        D2 = (tiltErr - P2)*10;
        P2 = tiltErr;
        I2 = (I2 + tiltErr * 0.01);
        
        if (P1*Kp1+D1*Kd1+I1*Ki1 > 255) {
            panSpeed = 255;
        }
        else if (P1*Kp1+D1*Kd1+I1*Ki1 < -255) {
            panSpeed = -255;
        }
        else {
            panSpeed = P1*Kp1+D1*Kd1+I1*Ki1;
        }

        if (P2+D2+I2 > 255) {
            tiltSpeed = 255;
        }
        else if (P2+D2+I2 < -255) {
            tiltSpeed = -255;
        }
        else {
            tiltSpeed = P2+D2+I2;
        }

        Motor1.driveMotor(abs(panSpeed), panSpeed > 0);
        //Motor2.driveMotor(abs(tiltSpeed), tiltSpeed < 0);
        Serial << "Pan Error: " << panErr << endl;
        vTaskDelay(10); // Task Period of 10 ms
    }
 }