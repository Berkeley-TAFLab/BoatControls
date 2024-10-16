//Predefined Libraries and headers
#include <Arduino.h>
#include <Wire.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>

//User defined libraries and headers
#include "constants.h"
#include "Boat_main.h"
#include "TAF_AS5600.h" 
#include "TAF_MPU6050.h"
#include "TAF_LIS3MDL.h"
#include "TAF_GTU7.h"
#include "TAF_Xbee.h"
#include "Boat_steer.h"

//Private user defines. These will likely be used by sensors in their respective source files
HardwareSerial Gps_Serial(1); //Variable used by the GPS
HardwareSerial Xbee_Serial(2); //Variable used by Xbee
Servo tailServo;
Servo sailServo;


//Variables for xbee 
uint8_t receive_buffer[256];
size_t receive_length;
uint64_t source_address;
uint16_t source_network_address;



//Detect if there's waterin the bottom of the boat, alert if so
void water_detection_task(void* parameter){
    while(1){
        //TODO: implement alert if water level is too high

        //From basic testing it seeems that anything greater than 2000 can probably be considered wet.
        int level = analogRead(WATER_LEVEL_PIN);
        // Serial.print("Water Level: ");
        // Serial.println(level); 

        //Delay task to every 100ms
        vTaskDelay(WATER_DETECTION_DELAY/portTICK_PERIOD_MS);
    }
}


void steering_task(void* parameter){
    while(1){
        //TODO: Finish the steering task. This should already mainly exist somewhere  
        SM_States curr_state = get_curr_state();
        switch (curr_state){
            case IDLE:
                Serial.println("Currently Idling... Do Nothing");
                break;
            case MANUAL: 
                manual_steer();
                Serial.println("Currently in Manual controls...");
                break;
            case AUTO:
                while(curr_state == AUTO){
                    Serial.println("Currently in Auto mode");
                    auto_steer();
                }
                break;
            default:
                Serial.println("Unknown Command...");
                break; 
        } 

        //Delay task to every 200ms
        vTaskDelay(STEERING_DELAY/portTICK_PERIOD_MS);
    }
}

void user_input_task(void* parameter){ 
    uint8_t count = 0; 
    uint8_t watchdog = 0;

    while(1){
        //This block is used by the boat to connect to a network on startup
        if(get_com_status() == SEARCHING){

            if(count == 5){
                uint8_t payload[] = {SEND_STATUS_MSG, get_curr_state()};
                transmit_xbee(payload, sizeof(payload));
                count = 0;
            }
            if (receive_xbee(receive_buffer, &receive_length, &source_address, &source_network_address))
            {
                Serial.println("received message");
                parse_xbee_msg(receive_buffer, receive_length);
                trans_com(CONNECTED);
            }
            count ++;

        //Once we connect to a network we can start responding properly instead of 
        //constantly transmitting 
        }else{ 

            //If we receive a message, reset watchdog and parse message properly
            if (receive_xbee(receive_buffer, &receive_length, &source_address, &source_network_address))
            {
                watchdog = 0;
                Serial.println("received message");
                parse_xbee_msg(receive_buffer, receive_length);

            //After USER_INPUT_Delay * 30 has elapsed and no response from GUI, we will go back 
            // to searching mode to find a network to connect to
            }else if(watchdog > 30){
                trans_com(SEARCHING);
            }else{
                watchdog++;
            }

        }


        vTaskDelay(USER_INPUT_DELAY/portTICK_PERIOD_MS);
    }
}

//Collect readings from the imu, encoder, magnetometer,gps
void sensor_readings_task(void* parameter){
    while(1){
        read_wind_vane();
        //Serial.println(get_avg_angle());
        read_mpu6050();
        read_lis3mdl();
        read_gtu7(); 


        //See constants.h file for delay settings
        vTaskDelay(SENSOR_READ_DELAY/portTICK_PERIOD_MS);

    }
}


//This task is primarily used in order to test that you're actually multithreading. 
//Feel free to experiment with it 
void test_serial_task(void* parameter){

    int degrees = 0;
    while(1){


        int new_degree = degrees % 135;
        sailServo.write(new_degree);
        tailServo.write(new_degree);
        degrees += 45;
        
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
} 

//Some peripherals get used by all sensors, this contains 
// those peripherals
void general_init(){

    //Used by all I2C devices
    Wire.begin(ENCODER_SDA_PIN,ENCODER_SCL_PIN);  
    Gps_Serial.begin(9600,SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    Xbee_Serial.begin(9600, SERIAL_8N1, XBEE_RX_PIN, XBEE_TX_PIN);
}

void main_setup(){
    //Initialize Serial Monitor
    Serial.begin(SERIAL_BAUD); 

    //Hardware peripheral initialization
    general_init();

    //Software peripheral initialization. Note not all sensors require this step
    windvane_init(); // This is needed because of the semaphores unfortunately. DON'T COMMENT OUT 
    setup_mpu6050(); // used for setup with the imu 
    setup_lis3mdl();
    setup_gtu7();
    setup_steer();
    sailServo.attach(SAIL_SERVO_PIN);
    tailServo.attach(TAIL_SERVO_PIN);
    
    //Initialize the state machine before doing anything
    sm_init();

    delay(1000); //Delay necessary in order to finish setting up peripherals
}