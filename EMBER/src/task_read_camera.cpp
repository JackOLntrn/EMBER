/** @file task_read_camera.h
 * task for reading the MLX90640 thermal camera
 * 
 * @author Jackson Cordova
 * @date November 2025
 */

#include "PrintStream.h"
#include "task_read_camera.h"
#include <Arduino.h>
//#include "MLX90640.h"
#include <Wire.h>
#include "shares.h"
#include "task_webserver.h"
#include <adafruit_mlx90640.h>


#define NUM_PIXELS 768

/** @brief function which outputs the index from  */
inline int idxFromXY(int x, int y) {
    return y * 32 + x;
}

Adafruit_MLX90640 mlx;

/** @brief task which reads the MLX90640 camera and saves the
 * most recent frame to a share
 * 
 * @details this task reads the MLX90640 camera over I2C and
 * saves the most recent frame. This task operates at
 * a frequency of 8 hz. This task also checks the frame for a hotspot
 * and sends the index of the hottest pixel to a share if it is above
 * 50 degrees C. This also indicates that a fire is detected by setting
 * the fire share to true. 
 */
void task_read_camera(void* p_params) {
    long n = 0;
    fire.put(false);
    Wire.begin(21,22);
    Wire.setClock(400000); // 400kHz I2C
    float highestTemp = 0;
    uint16_t maxIndex;
    uint8_t nframes = 0;
    
    if (!mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
        while(1) { Serial.println("MLX90640 not found"); delay(1000); }
    }
    mlx.setMode(MLX90640_CHESS);
    mlx.setResolution(MLX90640_ADC_18BIT);
    mlx.setRefreshRate(MLX90640_16_HZ); // try 8–16 Hz

    while(true) {
        if(!mlx.getFrame(Frame)) {
            //n++;
            //Serial << (millis()/1000) << " : " << n << endl;

            for (uint16_t i = 0; i < NUM_PIXELS; i++) {
                if (Frame[i] > highestTemp) {
                    highestTemp = Frame[i];
                    maxIndex = i;
                }
            }

            hotIndex.put(maxIndex);

            if (highestTemp > 50) {
                fire.put(true);
            }
            if (highestTemp < 45 && fire.get() && nframes > 5 ) {
                fire.put(false);
                nframes = 0;
            }
            else if (highestTemp < 45 && fire.get()) {
                nframes++;
            }
        }
    }
}