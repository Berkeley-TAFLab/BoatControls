//Predefined libraries and headers
#include <Arduino.h> 

//User defined libraries and headers
#include "TAF_Transmitter_Main.h" 
#include "constants.h" 

void main_setup(){
    Serial.begin(BAUD_RATE);

} 

//Since you can't transmit and receive wirelessly at the same time a mutex or semaphore should be used
void receive_boat(){
    while(1){


    }
}

void transmit_boat(){
    while(1){

    }

}

//Receives UART messages from the host computer 
void receive_UART(){

}

//Transmits messages to the host computer
void transmit_UART(){

} 



