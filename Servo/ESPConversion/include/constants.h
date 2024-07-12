//Dynamic Pin definitions, You may change these however you see fit 
#define LED_PIN 2
#define WATER_LEVEL_PIN 36 
#define SAIL_SERVO_PIN 13 
#define TAIL_SERVO_PIN 7 //This needs to be redefined  

#define RF_433_TX_PIN 39 //Transceiver pin for 433MHz transceiver module
#define RF_433_RX_PIN 34 //Receiver pin for 433 MHz receiver module 

#define GPS_RX_PIN 16 //From tx on gps to rx on esp32
#define GPS_TX_PIN 17 //From Rx on esp32 to tx on gps but you can maybe disregard this entirely

//Static pin definitions. You should try to refrain from changing these 
#define ENCODER_SDA_PIN 21 //Don't change
#define ENCODER_SCL_PIN 22 //Don't change

//Task priority definitions (These shouldn't really be changed much) 
#define WATER_DETECTION_PRIO 5
#define SENSOR_READ_PRIO     4
#define STEERING_PRIO        3
#define USER_INPUT_PRIO      2 
#define TEST_TASK_PRIO       1 

//Task delays. Change these however you see fit
#define WATER_DETECTION_DELAY 100
#define SENSOR_READ_DELAY     100
#define STEERING_DELAY        200
#define USER_INPUT_DELAY      500
#define TEST_TASK_DELAY       500 

//Peripheral setup
#define SERIAL_BAUD 9600 //Set to whatever you want. Platform.io has a default baudrate of 9600 

//Calibration values for the lis3mdl hard iron distortion 
const float lis3mdl_hard_iron_vals[3] = {
    -13.47,
    -13.94,
    -51.89
};

//Calibration values for lis3mdl soft iron distortion
const float lis3mdl_soft_iron_vals[3][3] = {
    {1.045 , 0.050 , 0.003},
    {0.050 , 0.998 , 0.004},
    {0.003 , 0.004 , 0.962}
}; 

//Values related to RF 433MHz transmission 
#define CP_ID 0x00 //Value of the control panel ID
#define RF_ID 0x01 // This value will change as more devices are added

//Message types for the RF 433MHz transmissions. More may be added 
#define SET_MODE 0x00 
#define STEER_MSG 0x01
#define POLL_STATUS 0x02 
#define POLL_LAT 0x03 
#define POLL_LONG 0x04 

//Debugging variables to disable certain setup tasks or functions