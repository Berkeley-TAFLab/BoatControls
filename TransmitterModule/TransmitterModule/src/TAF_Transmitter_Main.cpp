//Predefined libraries and headers
#include <Arduino.h> 

//User defined libraries and headers
#include "TAF_Transmitter_Main.h" 
#include "TAF_ControllerSide_433Mhz.h"
#include "constants.h"  

uint8_t buf[MAX_MESSAGE_LENGTH];

void main_setup(){
    Serial.begin(BAUD_RATE);

    //Call to setup the manager class for 433MHz radio
    setup_rf433();

} 

//Since you can't transmit and receive wirelessly at the same time a mutex or semaphore should be used

//Receive from the 433MHz radio module and store value 

void receive_boat(){
    receive_rf433(buf);
}

//Transmit the stored message from the uart receive through the 433MHz radio
void transmit_boat(){
    while(1){
    }

}

//Receives UART messages from the host computer 
void receive_UART(){
    while(1){
        //Receive UART messages from the GUI and store value
    }
}

//Transmits messages to the host computer
void transmit_UART(){
    while(1){


    }
} 



