/*
    This file is contains source code related to reading the windvane
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

uint8_t tick = 0;
uint16_t running_sum = 0;


//Reads current value from windvane and calculates average if possible
void read_wind_vane(){
    //
    if(xSemaphoreTake(last_read_angle_sem, 5000) == true){
        last_read_angle = as5600.readAngle();
        xSemaphoreGive(last_read_angle_sem);
    }else{
        Serial.println("Possible deadlock on windvane");
    }
    running_sum += last_read_angle; 
    tick++; 


    //TODO: Optimize this moving average filter. There's definitely
    //better ways of doing this. This sucks 
    if(tick > 5){
        if(xSemaphoreTake(avg_angle_sem, 5000) == true){
            avg_angle = running_sum / tick; 
            xSemaphoreGive(avg_angle_sem);
        }else{
            Serial.println("Possible deadlock on average angle");
        }
        tick = 0; 
        running_sum = 0;
    }
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
}


