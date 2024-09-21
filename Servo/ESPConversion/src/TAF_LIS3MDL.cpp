/*
    This file contains all source code related to using the LIS3MDL Magnetometer
*/

//Predefined libraries and headers 
#include <Adafruit_SensorLab.h>
#include <Arduino.h>

//User defined libraries and headers 
#include "TAF_LIS3MDL.h"
#include "constants.h" // Include for calibration values

//User defined variables 
Adafruit_SensorLab lab;

Adafruit_Sensor *mag = NULL;

sensors_event_t mag_event;

// static SemaphoreHandle_t magnetometer_data;
float mag_data[3] = {0,0,0};


void read_lis3mdl(){

    if(mag && ! mag->getEvent(&mag_event)){
        Serial.println("Could not receive mag data");
        return;
    }

    mag_data[0] = mag_event.magnetic.x; 
    mag_data[1] = mag_event.magnetic.y; 
    mag_data[2] = mag_event.magnetic.z; 

    // Serial.print("X_reading: ");
    // Serial.println(mag_event.magnetic.x);

    // Serial.print("Y_reading: ");
    // Serial.println(mag_event.magnetic.y);

    // Serial.print("Z_reading: ");
    // Serial.println(mag_event.magnetic.z);
    
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
    lab.begin();
    mag = lab.getMagnetometer();
    if(!mag){
        Serial.println("Could not find magnetometer");
    } 

} 
