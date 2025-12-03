/** @file task_read_camera.h
 * task for controlling system states
 * 
 * @author Jackson Cordova
 * @date December 2025
 */

#include "PrintStream.h"
#include "shares.h"


 
void task_stateMachine(void* p_params) {
    uint8_t state = 0;

    while(true) {
        
        switch(state) {
            case 0:
                // init state
                break;
            case 1:
                // scanning state
                break;
            case 2:
                // tracking state
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
