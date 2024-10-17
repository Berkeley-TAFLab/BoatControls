/*
    This file is contains source code related to reading the AS5600 Encoder.
*/

//Predefined Libraries 
#include <Arduino.h>
#include <AS5600.h>
#include <Wire.h>


//User defined libraries and headers
#include "TAF_AS5600.h" 
#include "constants.h"

//User defined variables and constants 
AS5600 as5600; 

static SemaphoreHandle_t last_read_angle_sem;
volatile uint16_t last_read_angle = 0; //Marked as volatile since other threads can access it 
static SemaphoreHandle_t avg_angle_sem;
volatile uint16_t avg_angle = 0; //Marked as volatile since other threads may access it 


volatile uint16_t angle_buffer[5] = {0}; // Buffer to store last 5 readings
volatile uint8_t buffer_index = 0; // Index to keep track of the current position in the buffer
volatile uint32_t running_sum = 0; // Running sum of the last 5 readings



//Reads current value from windvane and calculates average if possible
void read_wind_vane(){
    uint16_t current_angle; 
    if(xSemaphoreTake(last_read_angle_sem, 5000) == true){
            float raw_angle = as5600.readAngle();
        current_angle = (uint16_t)((raw_angle / 4095.0f) * 360.0f);
        last_read_angle = current_angle;
        xSemaphoreGive(last_read_angle_sem);
    }else{
        Serial.println("Possible deadlock on windvane");
    }
    if(xSemaphoreTake(avg_angle_sem, 5000) == true){
        // Subtract the oldest value from the running sum
        running_sum -= angle_buffer[buffer_index];
        
        // Add the new value to the buffer and the running sum
        angle_buffer[buffer_index] = current_angle;
        running_sum += current_angle;
        
        // Move to the next position in the circular buffer
        buffer_index = (buffer_index + 1) % 5;

        // Calculate new average
        avg_angle = running_sum / 5;

        xSemaphoreGive(avg_angle_sem);
    }else{
        Serial.println("Possible deadlock on average angle");
    }

    //TODO: Optimize this moving average filter. There's definitely
    //better ways of doing this. This sucks 

} 

//Shouldn't really ever use this unless debugging, but it's here if needed
uint16_t get_cur_angle(){
    uint16_t return_val = last_read_angle;
    if(xSemaphoreTake(last_read_angle_sem, 5000) == true){
        return_val = last_read_angle;
        xSemaphoreGive(last_read_angle_sem);
    }    

    //Return -1 if unable to receive semaphore
    return return_val;
}

//Returns the average measured angle. You should use this  when 
// making calculations since it's a "filtered" value
uint16_t get_avg_angle(){
    uint16_t return_val = -1;
    if(xSemaphoreTake(avg_angle_sem, 5000) == true){
        return_val = avg_angle;
        xSemaphoreGive(avg_angle_sem);
    }

    //Return -1 if unable to receive semaphore
    return return_val; 
} 

//Initialization function for any sensors that require it.
void windvane_init(){
    //Initialization required for AS5600 encoder
    as5600.begin();
    as5600.setDirection(AS5600_CLOCK_WISE);
    as5600.setOffset(0);
    last_read_angle_sem = xSemaphoreCreateMutex();
    avg_angle_sem = xSemaphoreCreateMutex(); 


    float raw_initial_angle = as5600.readAngle();
    uint16_t initial_angle = (uint16_t)((raw_initial_angle / 4095.0f) * 360.0f);
    for (int i = 0; i < 5; i++) {
        angle_buffer[i] = initial_angle;
    }
    running_sum = initial_angle * 5;
    avg_angle = initial_angle;
    last_read_angle = initial_angle;
}


