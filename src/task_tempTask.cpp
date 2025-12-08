/** @file task_tempTask.h
 * Task for appropriately setting the shared variables
 * for testing
 * 
 * @author Jackson Cordova
 * @date November 2025
 */

#include "task_tempTask.h"
#include <PrintStream.h>
#include "shares.h"


void task_tempTask(void* p_params) {
    fire.put(true);
    panRefCount.put(0);
    tiltRefCount.put(0);

    while(true) {
        if (millis() > 5000) {
            fire.put(true);
            panRefCount.put(4192);
            tiltRefCount.put(4192);
        }
        vTaskDelay(100);
    }
}