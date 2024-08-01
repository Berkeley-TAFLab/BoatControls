//Predefined libraries and headers
#include <Arduino.h> 

//User defined libraries and headers
#include "TAF_Transmitter_Main.h" 
#include "constants.h"

void main_setup(){
    Serial.begin(BAUD_RATE);

} 


//Receives UART messages from the host computer 
void receive_UART(){

}

//Transmits messages to the host computer
void transmit_UART(){

} 

void transmit_boat(){

}

//Testing task in order to ensure everything works
void test_serial_task(void* parameter){
    while(1){
        //Add something here. Uncomment below if you want
        // Serial.println("Hello World!");
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
} 