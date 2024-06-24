/*
    This file contains all source code related to using the LIS3MDL Magnetometer
*/

//Predefined libraries and headers
#include <Adafruit_LIS3MDL.h> 
#include <Arduino.h>

//User defined libraries and headers 
#include "TAF_LIS3MDL.h"
#include "constants.h" // Include for calibration values

//User defined variables 
Adafruit_LIS3MDL lis3mdl;  // Initialize magnetometer instance 

// static SemaphoreHandle_t magnetometer_data;
float mag_data[3] = {0,0,0};


void read_lis3mdl(){
    lis3mdl.read();  
    Serial.print("X_reading: ");
    Serial.println(lis3mdl.x);

    Serial.print("Y_reading: ");
    Serial.println(lis3mdl.y);

    Serial.print("Z_reading: ");
    Serial.println(lis3mdl.z);
    
    mag_data[0] = lis3mdl.x; 
    mag_data[1] = lis3mdl.y; 
    mag_data[2] = lis3mdl.z;
} 

float get_heading_lis3mdl(){
    static float hi_cal[3];
    static float heading = 0; 

    for (uint8_t i = 0; i < 3; i++ ) {
        hi_cal[i] = mag_data[i] - lis3mdl_hard_iron_vals[i];
    } 

      // Apply soft-iron scaling
    for (uint8_t i = 0; i < 3; i++) {
        mag_data[i] = (lis3mdl_soft_iron_vals[i][0] * hi_cal[0])+  
                    (lis3mdl_soft_iron_vals[i][1] * hi_cal[1])+  
                    (lis3mdl_soft_iron_vals[i][2] * hi_cal[2]);
    }
    // Calculate angle for heading, assuming board is parallel to
    // the ground and  Y points toward heading.
    heading = -1 * (atan2(mag_data[0], mag_data[1]) * 180) / M_PI;

    // Convert heading to 0..360 degrees
    if (heading < 0) {
        heading  = 360;
    }

    return heading;
} 


void setup_lis3mdl(){
    if(!lis3mdl.begin_I2C()){
        Serial.println("Unable to initialize magnetometer");
    }else{
        Serial.println("Initializd magnetometer");
    }  

    // magnetometer_data = xSemaphoreCreateMutex();

    //Hopefully these lines won't be needed but I'll keep them in case
    lis3mdl.setPerformanceMode(LIS3MDL_MEDIUMMODE);
    lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE); 
    lis3mdl.setDataRate(LIS3MDL_DATARATE_155_HZ);
    lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
    lis3mdl.setIntThreshold(500);



} 
