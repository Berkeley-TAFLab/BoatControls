#include "main.h"

//For now use only a single core when running code 
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0; 
#else 
static const BaseType_t app_cpu = 1; 
#endif 

void setup() {
  // put your setup code here, to run once:
  main_setup(); 

  //Initialize serial task
  xTaskCreatePinnedToCore(
    test_serial_task, 
    "Test Serial",
    1024, 
    NULL,
    TEST_SERIAL_PRIO,
    NULL,
    app_cpu
  );

}

void loop() {
  // put your main code here, to run repeatedly:
}

