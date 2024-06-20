#include "Boat_SM.h"

//Function to set up all peripherals and stuff.
void main_setup();


void steering_task(void* parameter); 

//Detects water and sends alert if water levels are too high
void water_detection_task(void* parameter); 
void sensor_readings_task(void* parameter); 
void manual_controls_task(void* parameter); 
void user_input_task(void* parameter);


//This task is just used to confirm if your multithreading works
void test_serial_task(void* parameter);
