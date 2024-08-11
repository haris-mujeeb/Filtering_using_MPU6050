/**
 * @file MPU6050.cpp
 * @brief Implementation of the MPU6050 class for interfacing with the MPU6050 accelerometer and gyroscope sensor.
 * 
 * This file provides a class implementation for initializing, reading sensor data,
 * and processing it from the MPU6050 sensor. The class supports basic operations like
 * angle calculation and complementary filtering.
 * 
 * @author Muhammad Haris Mujeeb
 * @author Dejan, https://howtomechatronics.com
 * @version 1.0
 * @date 2024-08-08
 */


#ifndef MPU6050_H
#define MPU6050_H

#include <Wire.h>

/**
 * @class MPU6050
 * @brief A class to interact with the MPU6050 accelerometer and gyroscope sensor.
 *
 * This class provides methods to initialize the sensor, read data from it, calculate angles,
 * and apply a complementary filter.
 */
class MPU6050 {
public:
  /**
   * @brief Constructor for MPU6050 class.
   * 
   * @param address The I2C address of the MPU6050 sensor. Defaults to 0x68 if not provided.
   */
  MPU6050(int address = 0x68);

  /**
   * @brief Initializes the MPU6050 sensor.
   *
   * This function sets up I2C communication, initializes the MPU6050 sensor, and 
   * configures it for reading accelerometer and gyroscope data.
   */
  void init();

  /**
   * @brief Reads raw accelerometer and gyroscope data from the MPU6050 sensor.
   *
   * This function reads data from the accelerometer and gyroscope registers of the MPU6050,
   * and stores the values in the class variables. The raw accelerometer data is converted
   * to g-force units, and the gyroscope data is converted to degrees per second.
   */
  void readSensor();

  /**
   * @brief Calculates the angles based on the gyroscope data.
   *
   * This function integrates the gyroscope data over time to calculate the 
   * angular displacement (angles) around the X, Y, and Z axes.
   */
  void calculateAngles();

  /**
   * @brief Applies a complementary filter to fuse accelerometer and gyroscope data.
   *
   * This function combines the angle calculated from the accelerometer with the 
   * angle calculated from the gyroscope using a complementary filter. The filter
   * is used to reduce the drift typically observed in gyroscope data over time.
   */
  void complementaryFilterFIR();

  float roll;  ///< The roll angle (rotation around X-axis) in degrees.
  float pitch; ///< The pitch angle (rotation around Y-axis) in degrees.
  float yaw;   ///< The yaw angle (rotation around Z-axis) in degrees.

private:
  int MPU_ADDRESS; ///< The I2C address of the MPU6050 sensor.
  const float GAIN = 0.96; ///< Gain for the complementary filter.
  
  // Raw data variables for accelerometer and gyroscope
  float AccX, AccY, AccZ;   
  float GyroX, GyroY, GyroZ;
  
  // Angle calculation variables
  float accAngleX, accAngleY;
  float gyroAngleX, gyroAngleY, gyroAngleZ;

  // Error correction offsets for accelerometer and gyroscope
  float AccErrorX, AccErrorY;
  float GyroErrorX, GyroErrorY, GyroErrorZ;

  // Time measurement variables for angle calculation
  float elapsedTime, currentTime, previousTime;
};

/**
 * @brief Constructor for MPU6050 class.
 * 
 * @param address The I2C address of the MPU6050 sensor. Defaults to 0x68 if not provided.
 */
MPU6050::MPU6050(int address) : MPU_ADDRESS(address) {}

void MPU6050::init() {
  // Initialize I2C communication
  Wire.begin();
  
  // Start communication with MPU6050 using the provided (or default) address
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B); // Talk to the power management register 6B
  Wire.write(0x00); // Set the sleep mode to 0 (wakes up the MPU6050)
  Wire.endTransmission(true);

    /*
  // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  */
  // Call this function if you need to get the IMU error values for your module
  // calculate_IMU_error();
} 

void MPU6050::readSensor() {
 // === Read acceleromter data === //
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDRESS, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX + 1.58; // GyroErrorX ~(-0.56)
  GyroY = GyroY - 0.47; // GyroErrorY ~(2)
  GyroZ = GyroZ + 0.60; // GyroErrorZ ~ (-0.8)
}

void MPU6050::calculateAngles() {
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
}

void MPU6050::complementaryFilterFIR() {
  // ... (FIR filtering implementation for roll)
  // Complementary filter - combine acceleromter and gyro angle values
  roll = GAIN * gyroAngleX + (1-GAIN) * accAngleX;
  pitch = GAIN * gyroAngleY + (1-GAIN) * accAngleY;
}

void calculate_IMU_error() {
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}

#endif // MPU6050_H