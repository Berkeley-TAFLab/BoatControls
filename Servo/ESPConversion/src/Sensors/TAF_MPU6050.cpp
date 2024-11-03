
//Predefined libraries and header files
#include <Arduino.h> 
#include <Wire.h> // THis should be removed later on
#include <MPU6050.h>  

//User defined libraries and header files
#include "TAF_MPU6050.h"

//User defined variables
MPU6050 mpu;  

//TODO: Figure out how to use vectors cause this is disgusting 
//Also you might need a mutex on these as well 
static SemaphoreHandle_t mpu6050_sema;
float ax_g = 0;
float ay_g = 0;     //Accelerometer data
float az_g = 0; 

float gx_dps = 0; 
float gy_dps = 0;   //Gyroscope data  
float gz_dps = 0; 

// float roll = 0; 
// float pitch = 0; 

void read_mpu6050(){
    int16_t ax, ay, az, gx, gy , gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy , &gz ); 
    
    //Convert accelerometer data to g 
    if(xSemaphoreTake(mpu6050_sema, 5000) == true){
        ax_g = ax / 16384.0;
        ay_g = ay / 16384.0;    //These conversion factors are basedo n setup
        az_g = az / 16384.0;   

        //Convert gyroscope data to degrees/sec
        gx_dps = gx / 131.0;
        gy_dps = gy / 131.0;    
        gz_dps = gz / 131.0; 
        xSemaphoreGive(mpu6050_sema);
    }else{
        Serial.println("Possible deadlock on mpu6050.");
    }


    //Test print methods
    // Serial.print("ACC_X: ");
    // Serial.println(ax_g);  

    // Serial.print("ACC_Y: ");
    // Serial.println(ay_g);

    // Serial.print("ACC_Z: ");
    // Serial.println(az_g);


    // Serial.print("GY_X: ");
    // Serial.println(gx_dps);  

    // Serial.print("GY_Y: ");
    // Serial.println(gy_dps);

    // Serial.print("GY_Z: ");
    // Serial.println(gz_dps);
    
}

//TODO: Access methods for the actual imu data. Not too sure what to do with
//that stuff yet 

void setup_mpu6050(){
    

    mpu.initialize(); 
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); 

    mpu6050_sema =  xSemaphoreCreateMutex();
    
}

