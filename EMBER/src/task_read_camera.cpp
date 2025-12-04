/** @file task_read_camera.h
 * task for reading the MLX90640 thermal camera
 * 
 * @author Jackson Cordova
 * @date November 2025
 */

#include "PrintStream.h"
#include "task_read_camera.h"
#include <Arduino.h>
#include <Wire.h>
#include "shares.h"
#include "task_webserver.h"
#include <adafruit_mlx90640.h>


#define NUM_PIXELS 768


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
/** Find the centroid and max temp of a connected blob of hot pixels
 *  Uses flood fill to find all pixels >= tempThreshold connected to seed
 *  Returns weighted average index (useful for positioning)
 */
void findHotBlob(const float* frame, uint16_t seedIdx, float tempThreshold, 
                 uint16_t& outCentroidIdx, float& outMaxTemp, float& outAvgTemp)
{
    // 32x24 grid
    const uint16_t W = 32;
    const uint16_t H = 24;
    
    static bool visited[NUM_PIXELS] = {false};
    memset(visited, 0, sizeof(visited));
    
    // BFS to find connected blob
    uint16_t queue[NUM_PIXELS];
    uint16_t qhead = 0, qtail = 0;
    
    queue[qtail++] = seedIdx;
    visited[seedIdx] = true;
    
    float sumTemp = 0;
    float sumWeightedX = 0;
    float sumWeightedY = 0;
    float totalWeight = 0;
    outMaxTemp = frame[seedIdx];
    
    while (qhead < qtail && qtail < NUM_PIXELS) {
        uint16_t idx = queue[qhead++];
        uint16_t x = idx % W;
        uint16_t y = idx / W;
        
        float temp = frame[idx];
        float weight = temp - (tempThreshold - 5);  // weight by how hot it is
        weight = max(0.0f, weight);
        
        sumTemp += temp;
        sumWeightedX += x * weight;
        sumWeightedY += y * weight;
        totalWeight += weight;
        outMaxTemp = max(outMaxTemp, temp);
        
        // Check 4-neighbors (up, down, left, right)
        // Up
        if (y > 0) {
            uint16_t nidx = (y - 1) * W + x;
            if (!visited[nidx] && frame[nidx] >= tempThreshold) {
                visited[nidx] = true;
                queue[qtail++] = nidx;
            }
        }
        // Down
        if (y < H - 1) {
            uint16_t nidx = (y + 1) * W + x;
            if (!visited[nidx] && frame[nidx] >= tempThreshold) {
                visited[nidx] = true;
                queue[qtail++] = nidx;
            }
        }
        // Left
        if (x > 0) {
            uint16_t nidx = y * W + (x - 1);
            if (!visited[nidx] && frame[nidx] >= tempThreshold) {
                visited[nidx] = true;
                queue[qtail++] = nidx;
            }
        }
        // Right
        if (x < W - 1) {
            uint16_t nidx = y * W + (x + 1);
            if (!visited[nidx] && frame[nidx] >= tempThreshold) {
                visited[nidx] = true;
                queue[qtail++] = nidx;
            }
        }
    }
    
    // Compute weighted centroid
    if (totalWeight > 0) {
        float centX = sumWeightedX / totalWeight;
        float centY = sumWeightedY / totalWeight;
        // Find nearest actual pixel to centroid
        float minDist = 1e9f;
        outCentroidIdx = seedIdx;
        for (uint16_t idx = 0; idx < NUM_PIXELS; idx++) {
            if (visited[idx]) {
                uint16_t x = idx % W;
                uint16_t y = idx / W;
                float dx = x - centX;
                float dy = y - centY;
                float dist = dx * dx + dy * dy;
                if (dist < minDist) {
                    minDist = dist;
                    outCentroidIdx = idx;
                }
            }
        }
        outAvgTemp = sumTemp / (float)qtail;  // average of all pixels in blob
    } else {
        outCentroidIdx = seedIdx;
        outAvgTemp = frame[seedIdx];
    }
}

void task_read_camera(void* p_params) {
    long n = 0;
    fire.put(false);
    Wire.begin(21,22);
    Wire.setClock(400000); // 400kHz I2C
    float highestTemp = 0;
    uint16_t maxIndex = 0;
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

            // Find the single hottest pixel first
            highestTemp = Frame[0];
            maxIndex = 0;
            for (uint16_t i = 1; i < NUM_PIXELS; i++) {
                if (Frame[i] > highestTemp) {
                    highestTemp = Frame[i];
                    maxIndex = i;
                }
            }

            // If hottest pixel is above threshold, find the hot blob around it
            if (highestTemp > 45) {
                uint16_t blobCentroid = maxIndex;
                float blobMaxTemp = highestTemp;
                float blobAvgTemp = highestTemp;
                
                // Find connected blob of pixels >= 40°C around the hotspot
                findHotBlob(Frame, maxIndex, 40.0f, blobCentroid, blobMaxTemp, blobAvgTemp);
                
                hotIndex.put(blobCentroid);
                
                if (blobMaxTemp > 50) {
                    fire.put(true);
                }
            } else {
                hotIndex.put(maxIndex);
                
                if (highestTemp > 50) {
                    fire.put(true);
                }
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