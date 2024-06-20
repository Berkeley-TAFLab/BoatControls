#include <Arduino.h>
#include "constants.h"
#include "Boat_main.h"
//Detect if there's waterin the bottom of the boat, alert if so
void water_detection_task(void* parameter){
    while(1){
        //TODO: implement alert if water level is too high
        int level = analogRead(WATER_LEVEL_PIN);
        Serial.print("Water Level: ");
        Serial.println(level); 

        //Delay task to every 100ms
        vTaskDelay(100/portTICK_PERIOD_MS);
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
                Serial.println("Currently in Manual controls...");
                break;
            case AUTO:
                Serial.println("Currently in Auto mode");
                break;
            default:
                Serial.println("Unknkown Command...");
                break; 
        }

        //Delay task to every 200ms
        vTaskDelay(200/portTICK_PERIOD_MS);
    }
}

void user_input_task(void* parameter){
    //TODO: As of Jun 18, use the serial monitor, later upgrade to an Xbee.
    // Maybe this stuff should be triggered via interrupt later
    while(1){
          if (Serial.available() > 0) {
            // read the incoming byte:
            String receiveString = Serial.readString(); 
            receiveString.trim(); 

            if(receiveString.equalsIgnoreCase("manual")){
                //Switch variable to manual control
                transition(MANUAL);
            }else if (receiveString.equalsIgnoreCase("auto")){
                //Switch variable to autonomous control
                transition(AUTO);
            }

        }

        //Delay task to every 500 ms
        vTaskDelay(500/portTICK_PERIOD_MS);

    }

}

//Collect readings from the imu, encoder, gps
void sensor_readings_task(void* parameter){
    while(1){
        
    }
}


//This task is primarily used in order to test that you're actually multithreading. 
//Feel free to experiment with it 
void test_serial_task(void* parameter){
    while(1){
        Serial.println("Testing serial task");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
} 


void main_setup(){
     // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000); //Delay necessary in order to finish setting up the serial monitor
  //Initialize the state machine before doing anything
  sm_init();
}