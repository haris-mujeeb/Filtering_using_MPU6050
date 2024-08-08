/**
 * @file main.cpp
 * @brief This file demonstrates the usage of the MPU6050 class for interfacing 
 * with an MPU6050 accelerometer and gyroscope sensor. It also shows how to use 
 * an IIR filter to smooth the roll angle calculated from the sensor data.
 * 
 * @author Your Name
 * @contributor Dejan, https://howtomechatronics.com
 */

#include "MPU6050.h"  // Include the header file for the MPU6050 class

/**
 * @brief Struct to hold parameters and output for an Infinite Impulse Response (IIR) filter.
 */
typedef struct {
    float alpha;  ///< Filter coefficient for current input
    float beta;   ///< Filter coefficient for previous output
    float output; ///< Filter output
} IIRFilter;

/**
 * @brief Initializes the IIR filter with specified coefficients.
 * 
 * @param filt Pointer to the IIRFilter struct to be initialized.
 * @param _alpha Coefficient for the current input.
 * @param _beta Coefficient for the previous output.
 */
void IIRFilter_Init(IIRFilter *filt, float _alpha, float _beta) {
    filt->alpha = _alpha;
    filt->beta = _beta;
    filt->output = 0.0f;
}

/**
 * @brief Updates the IIR filter with a new input value and returns the filtered output.
 * 
 * @param filt Pointer to the IIRFilter struct.
 * @param input The current input value to the filter.
 * @return float The filtered output.
 */
float IIRFilter_Update(IIRFilter *filt, float input) {
    // yn = alpha * xn - beta * y(n-1)
    filt->output = (filt->alpha * input) - (filt->beta * filt->output);
    return filt->output;   // Return filter output
}

// Global variables
IIRFilter myFilter;            ///< IIR filter instance for smoothing the roll angle
MPU6050 mpu(0x68);             ///< MPU6050 instance with I2C address 0x68

/**
 * @brief Arduino setup function. Initializes serial communication, the MPU6050 sensor, and the IIR filter.
 */
void setup() {
    Serial.begin(19200);             // Initialize serial communication at 19200 baud
    mpu.init();                      // Initialize MPU6050 sensor
    IIRFilter_Init(&myFilter, 1, 0.9);  // Initialize IIR filter with coefficients
}

/**
 * @brief Arduino loop function. Reads sensor data, calculates angles, applies filtering, and prints the results.
 */
void loop() {
    mpu.readSensor();                // Read data from the MPU6050 sensor
    mpu.calculateAngles();           // Calculate angles based on sensor data
    mpu.complementaryFilterFIR();    // Apply complementary filter to roll and pitch angles
    
    // Print the original roll angle
    Serial.print(mpu.roll);
    Serial.print("/");
    
    // Apply the IIR filter to the roll angle and print the filtered value
    mpu.roll = IIRFilter_Update(&myFilter, mpu.roll);
    Serial.println(mpu.roll);
}