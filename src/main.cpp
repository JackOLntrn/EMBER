/** 
 * E.M.B.E.R - main file
 * @author Jackson Cordova, Matthieu Gol, GPT-5 mini
 * @date November 2025
 * @description: Main file for the E.M.B.E.R project, initializes tasks and hardware components
 */
#include <Arduino.h>
#include "PrintStream.h"
#include "shares.h"
#include "taskshare.h"
#include "DRV8871Driver.h"
#include <adafruit_mlx90640.h>
#include <ESP32Encoder.h>
#include "task_motorControl.h"
#include "task_read_camera.h"
#include "task_webserver.h"
#include "task_stateMachine.h"

Share<int32_t> panRefCount("PanRefCount");
Share<int32_t> tiltRefCount("TiltRefCount");
Share<bool> fire("Fire");
Share<bool> spray("Spray");
Share<uint16_t> hotIndex("HotIndex");
Share<uint8_t> systemState("SystemState");
ESP32Encoder enc1;
ESP32Encoder enc2;
float Frame[768];
DRV8871Driver Motor1(16,17);
DRV8871Driver Motor2(12,13);

/** @brief Setup function that runs once at start of code and initializes tasks
 */
void setup() {
    
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);
    delay(1000);

    Motor2.brakeMotor();
    Motor1.brakeMotor();
    Serial.begin(115200);
    while(!Serial);
    delay(100);
    xTaskCreate(task_motorControl,"Motor Control Task",
                4096,
                NULL,
                5,
                NULL);
    xTaskCreate(task_read_camera,
                "Read Camera Task",
                8192,
                NULL,
                7,
                NULL);
    setup_wifi();
    xTaskCreate(task_webserver,"IoT Task",8192,NULL,1,NULL);

    xTaskCreate(task_stateMachine,
                "State Machine Task",
                4096,
                NULL,
                4,
                NULL);
}

/** @brief Loop function for Arduino framework - not used as tasks are created in setup()
 */
void loop() {
  vTaskDelay(60000);
}



