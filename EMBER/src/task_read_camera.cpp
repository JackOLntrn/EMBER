/** @file task_read_camera.h
 * task for reading the MLX90640 thermal camera
 * 
 * @author Jackson Cordova
 * @date November 2025
 */

#include "task_read_camera.h"
#include <Arduino.h>
#include "MLX90640.h"
#include "shares.h"

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
    static float Frame[NUM_PIXELS]; // place frame in static memory to avoid large stack usage

    while(true) {
        // check if new data is available and read if so
        if(mlx.checkNewDataAvailable()) {
            mlx.readFrame(Frame);
        }
        // printing for debugging

        // for(int i = 0; i < NUM_PIXELS; i++) {
        //     Serial << (int)Frame[i] << " ";
        //     if(i % 24 == 0) {
        //         Serial << endl;
        //     }
        // }
        // Serial << endl;

        for(int i = 0; i < NUM_PIXELS; i++) {
            // need to adjust this so that it finds average of all hot spots, while eliminating outliers
            // rather than looking for first hot pixel in array
            if (Frame [i] > 50.0) { 
                fire.put(true);
                Serial << "Hot Spot Detected at Pixel " << i << " with Temperature " << Frame[i] << endl;
                if(i+1 % 24 == 0) {
                    int n = (i / 24);
                    
                    // map index to angle away from center of vision

                }
            }
        }
            
        
        vTaskDelay(125);

    }

}