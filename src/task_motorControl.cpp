/** @file task_motorControl.cpp
 * 
 * @brief Motor control task for pan and tilt
 * @author Jackson Cordova, GPT5-mini
 * @date November 2025
 */

#include "task_motorControl.h"
#include <Arduino.h>
#include "DRV8871Driver.h"
#include <ESP32Encoder.h>
#include "PrintStream.h"
#include "shares.h"
#include <Preferences.h>

#define MAXSPEED 220
#define DEADBAND 10
#define Kp1 7
#define Kd1 4
#define Kp2 7
#define Kd2 4


 
 /** @brief task which controls the pan and tilt motors
  * @details This task reads the reference counts for pan and tilt from shares
  * as well as checks if a fire is present. When no fire is present, the task
  * uses a velocity control loop to sweep the pan motor back and forth between
  * its limits (180 degrees left and right). When a fire is detected, the task
  * switches to position control mode to move the pan and tilt motors to the
  * desired reference positions denoted by the reference counts from the shares.
  * The task uses proportional-derivative (PD) control for position control, and
  * proportional-integral-derivative (PID) control for velocity control during scanning.
  * The task has a period of 10 ms.
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
    
    // Scanning mode state
    static bool moveToMax = true; // direction flag for velocity sweep
    static long prevPanCount = 0;
    static float prevErr = 0;
    static float integralErr = 0;  // integral of velocity error
    const float Kpv = 10.0f;  // velocity proportional gain
    const float Kdv = 0.0f;  // velocity derivative gain
    const float Kiv = 2.0f;  // velocity integral gain
    const int8_t desiredVel = 8;  // desired velocity in counts/10ms
    const int16_t EDGE_THRESHOLD = 200;  // counts from edge to trigger direction flip

    while(true){

        // Read reference counts and clamp to encoder counts for 180 degree pan and 45 degree tilt
        rawPRC = panRefCount.get();
        rawTRC = tiltRefCount.get();
        if (rawPRC < -4764) {
            rawPRC = -4764;
        }
        if (rawTRC > 0) {
            rawTRC = 0;
        }
        if (rawTRC < -524) {
            rawTRC = -524;
        } 
        if (rawPRC > 4764) {
            rawPRC = 4764;
        }

        // Serial << "Raw PRC: " << rawPRC; // output for debugging

        // calculate position errors
        panErr = rawPRC - enc1.getCount();
        tiltErr = rawTRC - enc2.getCount();

        // Check if we're in scanning mode (when reference is at an extreme value)
        // If close to edge, use velocity control to sweep back and forth
        if ((rawPRC >= 4744 || rawPRC <= -4744) && !fire.get()) {
            // Scanning mode: velocity sweep between edges
            long currentPanCount = enc1.getCount();
            int8_t currentVel = (currentPanCount - prevPanCount) / 10;  // counts per 10ms tick
            int8_t desiredVelSigned = moveToMax ? desiredVel : -desiredVel;
            int8_t velErr = desiredVelSigned - currentVel;
            
            // Accumulate integral error
            integralErr += velErr;
            // Anti-windup: clamp integral term
            if (integralErr > 100.0f) integralErr = 100.0f;
            if (integralErr < -100.0f) integralErr = -100.0f;
            
            float dVel = velErr - prevErr;
            prevErr = velErr;
            
            panSpeed = (int16_t)(velErr * Kpv + dVel * Kdv + integralErr * Kiv);
            
            // Clamp to max speed
            if (panSpeed > MAXSPEED) {
                panSpeed = MAXSPEED;
            } else if (panSpeed < -MAXSPEED) {
                panSpeed = -MAXSPEED;
            }
            
            // Check for direction flip at edges
            if (currentPanCount >= 4764 - EDGE_THRESHOLD) {
                moveToMax = false;  // start moving toward min
                integralErr = 0;    // reset integral on direction flip
            } else if (currentPanCount <= -4764 + EDGE_THRESHOLD) {
                moveToMax = true;   // start moving toward max
                integralErr = 0;    // reset integral on direction flip
            }
            
            prevPanCount = currentPanCount;
        } else {
            // Fire mode or tracking mode: position control
            D1 = (panErr - P1)*10;
            P1 = panErr;
            panSpeed = P1*Kp1 + D1*Kd1;

            if (panSpeed > MAXSPEED) {
                panSpeed = MAXSPEED;
            }
            else if (panSpeed < -MAXSPEED) {
                panSpeed = -MAXSPEED;
            }
        }

        // Tilt always uses position control
        D2 = (tiltErr - P2)*10;
        P2 = tiltErr;
        tiltSpeed = P2*Kp2 + D2*Kd2;

        if(tiltSpeed > MAXSPEED) {
            tiltSpeed = MAXSPEED;
        }
        else if (tiltSpeed < -MAXSPEED) {
            tiltSpeed = -MAXSPEED;
        }


        Motor1.driveMotor(abs(panSpeed), panSpeed > 0);
        Motor2.driveMotor(abs(tiltSpeed), tiltSpeed < 0);
        //Serial << "Spraying " << endl;

        
        // output for debugging
        // int32_t tiltRef = tiltRefCount.get();
        // long tiltCount = enc2.getCount();
        // int32_t panRef = panRefCount.get();
        // long panCount = enc1.getCount();
        // Serial << "tilt ref: " << tiltRef << " | " << tiltCount << endl;
        // Serial << "TR: " << tiltRef << " | TC: " << tiltCount << " || PR: " << panRef << " | PC: " << panCount << endl;
            

        vTaskDelay(10); // Task Period of 10 ms
    }
 }