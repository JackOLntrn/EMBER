/** @file task_read_camera.h
 * task for reading the MLX90640 thermal camera
 * 
 * @author Jackson Cordova
 * @date November 2025
 */

#include "task_read_camera.h"
#include <Arduino.h>
#include "MLX90640.h"
#include <Wire.h>
#include "shares.h"
#include "task_webserver.h"

#define NUM_PIXELS 768

/** @brief task which reads the MLX90640 camera and saves the
 * most recent frame to a share
 * 
 * @details this task reads the MLX90640 camera over I2C and
 * saves the most recent frame to a share. This task operates at
 * a frequency of 8 hz. 
 */
void task_read_camera(void* p_params) {
    
    Wire.begin(); // begin I2C communication
    Wire.setClock(100000); // set I2C clock speed
    delay(100);
    static MLX90640_Interface mlx; // place instance in static memory to avoid large stack usage
    mlx.begin();

    while(true) {
        // check if new data is available and read if so into the global Frame buffer
        if(mlx.checkNewDataAvailable()) {
            mlx.readFrame(Frame);
        }

        // Check for hotspots and publish events
        for(int i = 0; i < NUM_PIXELS; i++) {
            if (Frame[i] > 50.0f) { 
                fire.put(true);
                Serial << "Hot Spot Detected at Pixel " << i << " with Temperature " << Frame[i] << endl;
            }
        }
        
        vTaskDelay(125);
    }

}