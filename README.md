# BoatControls
Arduino project that controls the rudder and sail based on the position of the wind vane. This document will be used in order to log current progress of the firmware.

# Devlog  

## 5/17/24
First message in the devlog, but at the moment we've converted the sail to move based on cubic polynomial. At the moment there are still some issues with the sail making excessive movements as it 
moves from 0 degrees to 360 degrees (since the encoder resets). A possible solution that I've considered is to set the "0" to the back since that excess movement is less important in the rear. 
Firmware wise, the boat should basically be ready to test 

## 5/20/24
Created folders for servo and stepper controls. Will keep organizing code within those two folders. Future devlog updates will also probably go in the readmes in those two folders.   

## 6/20/24 
After experimenting with ESP32 and FreeRTOS for a while I've finally been able to setup a proper project that we can use to move away from the Arduinos. Uploaded files in the Servo folder
Added header and source files for AS5600 Encoder in RTOS project. 

## 6/21/24 
Added header and source files for MPU6050 IMU and LIS3MDL Magnetometer in RTOS project 

## 6/26/24 
Source file for MPU6050 completed a few days ago. LIS3MDL still needs debugging as we had issues communicating with it through I2C. Some calibration might be necessary. 

## 6/27/24 
LIS3MDL was finished yesterday. Turns out previous unit might have been broken? There was some issues using the standard LIS3MDL library so I resorted to using the Adafruit Sensor Lab library.
Some calibration constants were updated. They should be updated again as our breadboard is put into the actual boat. Finished adding in GPS to the source files as well. Next up is the 433 MHz radio
I should also start trying to get a GUI up and running that would be pretty cool. Also figure out some comm protocol to send data.
