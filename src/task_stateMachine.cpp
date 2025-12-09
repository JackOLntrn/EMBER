/** @file task_stateMachine.cpp
 * task for controlling system states
 * 
 * @brief State machine task that manages system operation modes
 * @author Jackson Cordova, GPT5-mini
 * @date December 2025
 */

#include "PrintStream.h"
#include "shares.h"

#define CPP 17.5 // counts per pixel
#define WIDTH 32 // number of pixels in width
#define HEIGHT 24 // number of pixels in height


/** @brief State machine task
  * @details This task manages the different states of the system. 
  * State 0 is an initialization state which sets all shares to default. 
  * State 1 is a scanning state where the system looks back and forth for hot spots.
  * State 2 is a tracking state where the system points the thermal camera and hose
  * towards the detected hot spot and activates the spray mechanism. The task
  * period is set to 100 ms.
  * 
  * @param p_params not used and should be set to void in the xTaskCreate call
  */
void task_stateMachine(void* p_params) {
    uint8_t state = 0;
    uint8_t hour = 0;
    uint8_t panInd, tiltInd;
    pinMode(5, OUTPUT); // solenoid pin for spraying 
    digitalWrite(5, LOW);
    bool one_time_val = false;
    long now;
    int16_t panIndAway;

    while(true) {
        // Serial << "state: " << state << " ";
        switch(state) {
            case 0:
                // init state
                panRefCount.put(-4764);
                tiltRefCount.put(0);
                fire.put(false);
                spray.put(false);
                Serial << "System Initialized" << endl;
                digitalWrite(5, LOW);
                state = 1;
                
                break;
            case 1:
                if (fire.get()) {
                    Serial << "Fire Detected" << endl;
                    state = 2;
                } 
                tiltRefCount.put(0);
                Serial << "scanning... " << endl;
                if (enc1.getCount() > 4744) {
                    panRefCount.put(-4764);
                }
                if (enc1.getCount() < -4744) {
                    panRefCount.put(4764);
                }
                

                break;
            case 2:
                tiltInd = hotIndex.get() % WIDTH; // getting pixel index in tilt direction
                panInd = hotIndex.get() / WIDTH; // getting pixel index in pan direction 

                // setting pan and tilt reference counts based on hot spot location

                panIndAway = (panInd-HEIGHT/2);
                if (abs(panIndAway) < 3) {
                    panIndAway = 0;
                }

                panRefCount.put(enc1.getCount() + panIndAway*CPP*50/22);
                
                if (enc2.getCount() > -20) {
                    tiltRefCount.put(-524);
                }
                else if (enc2.getCount() < -504) {
                    tiltRefCount.put(0);
                }


                if (panIndAway == 0 && fire.get() && !one_time_val) {
                    Serial << "Activating Spray!" << endl;
                    spray.put(true);
                    digitalWrite(5, HIGH); // activate solenoid
                    one_time_val = true;
                    now = millis();
                }

                if (millis() - now > 10000 && one_time_val) {
                    Serial << "Deactivating Spray!" << endl;
                    spray.put(false);
                    digitalWrite(5, LOW); // deactivate solenoid
                    one_time_val = false;
                    Motor2.brakeMotor();
                }

                if (!fire.get() && !spray.get()) {
                    Serial << "Fire Out, Returning to Scan" << endl;
                    panRefCount.put(-4764);
                    tiltRefCount.put(0);
                    state = 1;
                }
                break;
            default:
                // more stuff
                break;
        }
        vTaskDelay(100);
    }

}
