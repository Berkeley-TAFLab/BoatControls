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
    5,
    NULL,
    app_cpu
  );

    //Initialize Intialize User Input Task
  xTaskCreatePinnedToCore(
    user_input_task, 
    "User Input",
    1024, 
    NULL,
    2,
    NULL,
    app_cpu
  );

  //Initialize Intialize Steering Task
  xTaskCreatePinnedToCore(
    steering_task, 
    "Steering",
    1024, 
    NULL,
    3,
    NULL,
    app_cpu
  );

  xTaskCreatePinnedToCore(
    sensor_readings_task,
    "Sensors",
    1024,
    NULL,
    4,
    NULL,
    app_cpu
  );

  //Initialize Serial task
  xTaskCreatePinnedToCore(
    test_serial_task, 
    "Test Task",
    1024, 
    NULL,
    1,
    NULL,
    app_cpu
  );




} 

void loop() {
  // put your main code here, to run repeatedly: 

}

