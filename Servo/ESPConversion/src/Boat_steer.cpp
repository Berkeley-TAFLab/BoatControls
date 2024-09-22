#include <Arduino.h>
#include <ESP32Servo.h>
#include <cmath>

// External servos defined in Boat_main
extern Servo tailServo;
extern Servo sailServo;

// Sail position requested by user
static SemaphoreHandle_t sail_sem;
uint8_t desired_sail_position;

// Rudder position requested by user
static SemaphoreHandle_t rudder_sem;
uint8_t desired_rudder_position;

uint8_t curr_sail_position;
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
        move_servo(sailServo, curr_sail_position, local_desired_sail);
    }

    if (local_desired_rudder != curr_rudder_position) {
        move_servo(tailServo, curr_rudder_position, local_desired_rudder);
    }
}

void auto_steer() {
    // Implement auto-steering logic here
    // This function should calculate desired positions based on sensor data
    // and then use the manual_steer() function to move the servos
}