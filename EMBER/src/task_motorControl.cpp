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
#include <Preferences.h>

#define MAXSPEED 180
#define DEADBAND 10
#define Kp1 7
#define Kd1 4
#define Kp2 7
#define Kd2 4


 
 /** @brief task which controls the pan and tilt motors
  * @details when a fire is not present, this task will drive 
  * the motors in a sinusoidal pattern. When a fire is present,
  * this task will drive the motors to a reference position
  * specified by the panRefCount and tiltRefCount shares.
  * 
  * @param p_params not used and should be set to void in the xTaskCreate call
  */
 void task_motorControl(void* p_params) {

    
    //initialize motors and encoders
    
    enc2.attachHalfQuad(32,33);
    enc1.attachHalfQuad(34,35);
    // ensure motors are stopped at startup
    Motor1.brakeMotor();
    Motor2.brakeMotor();

    int32_t panErr, tiltErr, rawPRC, rawTRC;
    int32_t P1, D1, P2, D2;
    P1 = 0;
    P2 = 0;
    int16_t panSpeed, tiltSpeed;

    while(true){
        rawPRC = panRefCount.get();
        rawTRC = tiltRefCount.get();
        if (rawPRC < -4764) {
            rawPRC = -4764;
        }
        if (rawTRC < 0) {
            rawTRC = 0;
        }
        if (rawTRC > 1048) {
            rawTRC = 1048;
        } 
        if (rawPRC > 4764) {
            rawPRC = 4764;
        }

        if (spray.get()) {
            // spray motion
            // need to tune these values
            //panSpeed = MAXSPEED * sin(millis() / 1000);
            panErr = rawPRC -enc1.getCount();
            tiltErr = rawTRC - enc2.getCount();

            D1 = (panErr - P1)*10;
            P1 = panErr;
            D2 = (tiltErr - P2)*10;
            P2 = tiltErr;

            panSpeed = P1*Kp1 + D1*Kd1;
            tiltSpeed = P2*Kp2 + D2*Kd2;

            
            if (panSpeed > MAXSPEED) {
                panSpeed = MAXSPEED;
            }
            else if (panSpeed < -MAXSPEED) {
                panSpeed = -MAXSPEED;
            }

            if(tiltSpeed > MAXSPEED) {
                tiltSpeed = MAXSPEED;
            }
            else if (tiltSpeed < -MAXSPEED) {
                tiltSpeed = -MAXSPEED;
            }

            

            Motor1.driveMotor(abs(panSpeed), panSpeed > 0);
            Motor2.driveMotor(abs(tiltSpeed), tiltSpeed > 0);
            //Serial << "Spraying " << endl;
        }
        else if (fire.get()) {
            

            panErr = rawPRC - enc1.getCount();
            tiltErr = rawTRC - enc2.getCount();
            // Serial << "PE: " << panErr << " ";

            D1 = (panErr - P1)*10;
            P1 = panErr;
            D2 = (tiltErr - P2)*10;
            P2 = tiltErr;

            panSpeed = P1*Kp1 + D1*Kd1;
            tiltSpeed = P2*Kp2 + D2*Kd2;

            // clamp motor speeds to MAXSPEED
            if(tiltSpeed > MAXSPEED) {
                tiltSpeed = MAXSPEED;
            }
            else if (tiltSpeed < -MAXSPEED) {
                tiltSpeed = -MAXSPEED;
            }

            if (panSpeed > MAXSPEED) {
                panSpeed = MAXSPEED;
            }
            else if (panSpeed < -MAXSPEED) {
                panSpeed = -MAXSPEED;
            }

            // drive motors, coasting if within deadband
            if (abs(panErr) < DEADBAND) {
                Motor1.coastMotor();
            }
            else {
                Motor1.driveMotor(abs(panSpeed), panSpeed > 0);
            }
            
            if (abs(tiltErr) < DEADBAND) {
                Motor2.coastMotor();
            }
            else {
                Motor2.driveMotor(abs(tiltSpeed), tiltSpeed > 0);
            }
        }
        else {
            Motor1.coastMotor();
            Motor2.coastMotor();

           
        }
        int32_t tiltRef = tiltRefCount.get();
        long tiltCount = enc2.getCount();
        int32_t panRef = panRefCount.get();
        long panCount = enc1.getCount();
        //Serial << "tilt ref: " << tiltRef << " | " << tiltCount << endl;
        // Serial << "TR: " << tiltRef << " | TC: " << tiltCount << " || PR: " << panRef << " | PC: " << panCount << endl;
            

        vTaskDelay(10); // Task Period of 10 ms
    }
 }