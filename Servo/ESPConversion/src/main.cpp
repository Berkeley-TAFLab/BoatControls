#include "main.h"

//For now use only a single core when running code 
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0; 
#else 
static const BaseType_t app_cpu = 1; 
#endif 

void setup() {

  //Function to setup required Peripherals
  main_setup(); 
  
  //Initialize water detection task
  xTaskCreatePinnedToCore(
    water_detection_task, 
    "Water Detection",
    1024, 
    NULL,
    WATER_DETECTION_PRIO,
    NULL,
    app_cpu
  );

    //Initialize Intialize User Input Task
  xTaskCreatePinnedToCore(
    user_input_task, 
    "User Input",
    4096, 
    NULL,
    USER_INPUT_PRIO,
    NULL,
    app_cpu
  );

  //Initialize Intialize Steering Task
  xTaskCreatePinnedToCore(
    steering_task, 
    "Steering",
    4096, 
    NULL,
    STEERING_PRIO,
    NULL,
    app_cpu
  );

  xTaskCreatePinnedToCore(
    sensor_readings_task,
    "Sensors",
    4096,
    NULL,
    SENSOR_READ_PRIO,
    NULL,
    app_cpu
  );

  // Initialize Serial task
  // xTaskCreatePinnedToCore(
  //   test_serial_task, 
  //   "Test Task",
  //   4096, 
  //   NULL,
  //   1,
  //   NULL,
  //   app_cpu
  // );




} 

//Ignore this section. Because we are using RTOS we should never have to do anything here
void loop() {
  // put your main code here, to run repeatedly: 

}

