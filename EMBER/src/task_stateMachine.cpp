/** @file task_read_camera.h
 * task for controlling system states
 * 
 * @author Jackson Cordova
 * @date December 2025
 */

#include "PrintStream.h"
#include "shares.h"

#define CPP 17.5 // counts per pixel
#define WIDTH 32
#define HEIGHT 24


/** @brief State machine task
  * @details This task manages the different states of the system. 
  * State 0 is an initialization state which sets all shares to default. 
  * State 1 is a scanning state where the system looks back and forth for hot spots.
  * State 2 is a tracking state where the system points the thermal camera and hose
  * towards the detected hot spot and activates the spray.
  * 
  * @param p_params not used and should be set to void in the xTaskCreate call
  */
void task_stateMachine(void* p_params) {
    uint8_t state = 0;
    uint8_t hour = 0;
    uint8_t panInd, tiltInd;
    pinMode(5, OUTPUT); // solenoid pin for spraying 
    bool one_time_val = false;
    long now;
    int16_t panIndAway;

    while(true) {
        // Serial << "state: " << state << " ";
        switch(state) {
            case 0:
                // init state
                panRefCount.put(0);
                tiltRefCount.put(0);
                fire.put(false);
                spray.put(false);
                Serial << "System Initialized" << endl;
                state = 1;
                
                break;
            case 1:
                if (fire.get()) {
                    Serial << "Fire Detected" << endl;
                    state = 2;
                } 

                Serial << "scanning... " << endl;
                

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
                
                if (abs(enc2.getCount()) < 10) {
                    tiltRefCount.put(524);
                }
                else if (abs(enc2.getCount() - 524) < 10) {
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
                
                

                break;
            case 3:
                // stuff
                break;
            default:
                // more stuff
                break;
        }

        vTaskDelay(100);
    }

}
