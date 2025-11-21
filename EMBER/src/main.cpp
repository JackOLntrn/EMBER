/** 
 * E.M.B.E.R - main file
 * @author Jackson Cordova, Matthieu Gol, GPT-5 mini
 * @date November 2025
 * @description: ...
 */
#include <Arduino.h>
#include "PrintStream.h"
// #include "shares.h"
// #include "taskshare.h"
// #include "DRV8871Driver.h"
// #include <ESP32Encoder.h>
// #include "task_motorControl.h"
// #include "task_tempTask.h"
#include "MLX90640.h"
#include "task_read_camera.h"

// Share<int32_t> panRefCount("PanRefCount");
// Share<int32_t> tiltRefCount("TiltRefCount");
// Share<bool> fire("Fire");



void setup() {
    Serial.begin(115200);
    while(!Serial);
    // xTaskCreate(task_motorControl,
    //             "Motor Control Task",
    //             4096,
    //             NULL,
    //             1,
    //             NULL);
    // xTaskCreate(task_tempTask,
    //             "Temp Task",
    //             4096,
    //             NULL,
    //             1,
    //             NULL);
    xTaskCreate(task_read_camera,
                "Read Camera Task",
                8192,
                NULL,
                1,
                NULL);
}

void loop() {
  vTaskDelay(60000);
}



