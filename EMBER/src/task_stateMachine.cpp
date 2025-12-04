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
    int32_t rawPanCount, rawTiltCount;
    pinMode(5, OUTPUT); // solenoid pin for spraying 
    bool one_time_val = false;
    long now;

    while(true) {
        
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
                delay(500);
                fire.put(true);
                // scanning state
                break;
            case 2:
                Serial << "attacking the fire" << endl;
                tiltInd = hotIndex.get() % WIDTH; // getting pixel index in tilt direction
                panInd = hotIndex.get() / WIDTH; // getting pixel index in pan direction 

                // setting pan and tilt reference counts based on hot spot location

                rawPanCount = enc1.getCount() + (panInd - WIDTH / 2) * CPP*4;
                rawTiltCount = enc2.getCount() + (tiltInd - HEIGHT / 2) * CPP;

                if (rawPanCount < 0) rawPanCount = 0;
                if (rawTiltCount < 0) rawTiltCount = 0;
                if (rawTiltCount > 1048) rawTiltCount = 1048; // limit tilt to max count
                if (rawPanCount > 4192*4) rawPanCount = 4192*4; // limit pan to max count

                panRefCount.put(rawPanCount);
                tiltRefCount.put(rawTiltCount);

                // if hot spot is near center, begin spraying
                if (panInd > (WIDTH / 2 - 2) && panInd < (WIDTH / 2 + 2) &&
                    tiltInd > (HEIGHT / 2 - 2) && tiltInd < (HEIGHT / 2 + 2)) {
                    spray.put(true);
                    digitalWrite(5, HIGH); // activate solenoid
                    if (!one_time_val) {
                        now = millis();
                        one_time_val = true;
                    }
                } 
                
                // spray for 2 seconds and check if fire is still present
                if (fire.get() == false && (millis() - now) > 2000) {
                    spray.put(false);
                    digitalWrite(5, LOW); 
                    one_time_val = false;
                    state = 1;
                }

                break;
            case 3:
                // stuff
                break;
            default:
                // more stuff
                break;
        }

        vTaskDelay(10);
    }

}
