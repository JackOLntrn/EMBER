/** @file main.cpp
 *  This is a simple test program for the MLX90640 thermal camera.
 *  It uses the MLX90640_Interface class defined in mlx90640.h, which is based
 *  on the MLX90640 API and I2C driver that were supplied by SparkFun and
 *  originally developed by Melexis.
 *  @author Spluttflob
 *  @date  2025-Oct-05
 */

#include <Arduino.h>
#include <mlx90640.h>


// Create an interface to the MLX90640
MLX90640_Interface mlx90640;


/** Setup function. Starts I2C and serial communication; initializes MLX90640.
 *  Halts if the MLX90640 is not found or if parameter extraction fails.
 */
void setup() 
{
    // Array to hold the raw EEPROM data
    uint16_t eeMLX90640[832];

    Serial.begin(115200);
    while(!Serial);
    delay(1000);
    Serial << "Testing a MLX90640 thermal camera" << endl;

    // Initialize I2C and set frequency
    Wire.begin();
    Wire.setClock(100000);
    delay(100);

    // Initialize the MLX90640 camera; hang here if it fails
    mlx90640.begin();
    Serial << "MLX90640 initialized." << endl;
}


/** @brief Main loop.
 *  Reads two frames from the MLX90640, merges them, and displays as ASCII art.
 */
void loop() 
{
    static float frame[NUM_PIXELS];

    mlx90640.readFrame(frame);
    mlx90640.displayFrameAsASCII(frame);
    Serial.println();
    delay(5000);

}