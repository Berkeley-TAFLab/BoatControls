//Pin definitions 
#define LED_PIN 2
#define WATER_LEVEL_PIN 36 
#define SAIL_SERVO_PIN 13 
#define TAIL_SERVO_PIN 21  


//Task priority definitions (These shouldn't really be changed much) 
#define WATER_DETECTION_PRIO 5
#define SENSOR_READ_PRIO     4
#define STEERING_PRIO        3
#define USER_INPUT_PRIO      2 
#define TEST_TASK_PRIO       1