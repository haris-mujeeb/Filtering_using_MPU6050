#include "MPU6050.h"  // Include the header file for the MPU6050 class

const int MAX_ORDER = 20;

typedef struct {
  float coefficients[MAX_ORDER];
  float history[MAX_ORDER];
  uint8_t head;
  uint8_t order;
} FIRFilter;

void FIRFilter_Init ( FIRFilter *filt, float coeffs[], float order) {
  filt->order = order;
  filt->head = 0;
  for (int i=0; i<order; i++) {
    filt->coefficients[i] = coeffs[i];
    filt->history[i]  = 0.0f;
  }
}

float FIRFilter_Update ( FIRFilter *filt, float input ) {
  filt->history[filt->head] = input;
  filt->head = (filt->head + 1) % filt->order;
  // Serial.print("h/ ");
  // Serial.println(filt->head);
  // Convolution of coefficients with input history     
  float output = 0.0f;
  for (int i = 0; i < filt->order; i++) {
      output += filt->coefficients[i] * filt->history[i];
  }
  return output;
}

MPU6050 mpu;
FIRFilter myFilter;

void setup() {
  Serial.begin(19200);             // Initialize serial communication at 19200 baud
  mpu.init();                      // Initialize MPU6050 sensor
  float coeffs_19th_order[] = {               // calculated using Octave or MatLab
    -0.0016631, -0.0040771, -0.0081140, -0.0096667,  0.0000000,  0.0289940, 
     0.0777010,  0.1350800,  0.1819100,  0.2000000,  0.1819100,  0.1350800, 
     0.0777010,  0.0289940,  0.0000000, -0.0096667, -0.0081140, -0.0040771, 
    -0.0016631 };

    float coeffs_5th_order[] = {               // calculated using Octave or MatLab
    0.012109,   0.101033,   0.200000,   0.101033,   0.012109 
    };

  FIRFilter_Init( &myFilter, coeffs_5th_order, 5) ; 
}

void loop() {
    mpu.readSensor();                // Read data from the MPU6050 sensor
    mpu.calculateAngles();           // Calculate angles based on sensor data
    mpu.complementaryFilterFIR();    // Apply complementary filter to roll and pitch angles
    
    // Print the original roll angle
    Serial.print(mpu.roll);
    Serial.print("/");
    
    // Apply the IIR filter to the roll angle and print the filtered value
    mpu.roll = FIRFilter_Update(&myFilter, mpu.roll);
    Serial.println(mpu.roll);
}