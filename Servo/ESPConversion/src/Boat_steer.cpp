#include <Arduino.h>
#include <ESP32Servo.h>
#include <cmath>
#include <TAF_AS5600.h>
#include <TAF_GTU7.h>
#include <TAF_LIS3MDL.h>
#include <TAF_MPU6050.h>
#include <Boat_steer.h>
#include "constants.h"

// External servos defined in Boat_main
extern Servo tailServo;
extern Servo sailServo;

// Sail position requested by user
static SemaphoreHandle_t sail_sem;
uint8_t desired_sail_position;

// Rudder position requested by user
static SemaphoreHandle_t rudder_sem;
uint8_t desired_rudder_position;

static SemaphoreHandle_t curr_sail_sem;
uint8_t curr_sail_position;

static SemaphoreHandle_t curr_rudder_sem;
uint8_t curr_rudder_position;

// Constants for cubic motion
const float MOTION_DURATION = 1000.0; // Duration of motion in milliseconds
const float MOTION_STEP = 20.0; // Update interval in milliseconds

void setup_steer() {
    desired_sail_position = 0;
    desired_rudder_position = 0;
    curr_sail_position = 0;
    curr_rudder_position = 0;
    sail_sem = xSemaphoreCreateMutex();
    rudder_sem = xSemaphoreCreateMutex();
    curr_sail_sem = xSemaphoreCreateMutex(); 
    curr_rudder_sem = xSemaphoreCreateMutex();
}

void set_sail_servo(uint8_t position) {
    if (xSemaphoreTake(sail_sem, 5000) == true) {
        desired_sail_position = position;
        xSemaphoreGive(sail_sem);
    }
}

void set_rudder_servo(uint8_t position) {
    if (xSemaphoreTake(rudder_sem, 5000) == true) {
        desired_rudder_position = position;
        xSemaphoreGive(rudder_sem);
    }
}

float cubic_ease(float t) {
    return t * t * (3.0 - 2.0 * t);
}

void move_servo(Servo &servo, uint8_t &current_position, uint8_t target_position) {
    float start_time = millis();
    float start_position = current_position;
    float distance = target_position - start_position;

    while (millis() - start_time < MOTION_DURATION) {
        float t = (millis() - start_time) / MOTION_DURATION;
        float eased_t = cubic_ease(t);
        float new_position = start_position + (distance * eased_t);

        servo.write(round(new_position));
        current_position = round(new_position);

        vTaskDelay(MOTION_STEP / portTICK_PERIOD_MS);
    }

    // Ensure final position is exactly the target
    servo.write(target_position);
    current_position = target_position;
}

void manual_steer() {
    uint8_t local_desired_sail;
    uint8_t local_desired_rudder;

    if (xSemaphoreTake(sail_sem, 5000) == true) {
        local_desired_sail = desired_sail_position;
        xSemaphoreGive(sail_sem);
    }

    if (xSemaphoreTake(rudder_sem, 5000) == true) {
        local_desired_rudder = desired_rudder_position;
        xSemaphoreGive(rudder_sem);
    }

    if (local_desired_sail != curr_sail_position) {
        uint8_t target_sail = (uint8_t)(local_desired_sail * SAIL_CAL_VAL);
        // Serial.print("Desired Rudder position: ");
        // Serial.println(target_sail);

        if (xSemaphoreTake(curr_sail_sem, 5000) == true) {
            move_servo(sailServo, curr_sail_position, target_sail);
            xSemaphoreGive(curr_sail_sem);
        }
    }

    if (local_desired_rudder != curr_rudder_position) { 
        uint8_t target_rudder = (uint8_t)(local_desired_rudder * RUDDER_CAL_VAL);
        // Serial.print("Desired Rudder position: ");
        // Serial.println(target_rudder); 

        if (xSemaphoreTake(curr_rudder_sem, 5000) == true) {
            move_servo(tailServo, curr_rudder_position, target_rudder);
            xSemaphoreGive(curr_rudder_sem);
        }
    }
}




uint8_t get_rudder_position(){
    uint8_t return_val = -1;
    if (xSemaphoreTake(curr_rudder_sem, 5000) == true) {
        return_val = curr_rudder_position;
        xSemaphoreGive(curr_rudder_sem);
    }

    return return_val;
}

void auto_steer() {
}
