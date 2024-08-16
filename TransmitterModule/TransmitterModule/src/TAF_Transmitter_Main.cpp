//Predefined libraries and headers
#include <Arduino.h> 

//User defined libraries and headers
#include "TAF_Transmitter_Main.h" 
#include "TAF_ControllerSide_433Mhz.h"
#include "constants.h"  

//User defined private variables
uint8_t boat_buf[MAX_MESSAGE_LENGTH]; //Buffer used to store messages from the boats
uint8_t gui_buf[MAX_MESSAGE_LENGTH]; //Buffer used to store messages from the GUI


void main_setup(){
    Serial.begin(BAUD_RATE);

    //Call to setup the manager class for 433MHz radio
    setup_rf433();

} 

//Since you can't transmit and receive wirelessly at the same time a mutex or semaphore should be used

//Receive from the 433MHz radio module and store value 
void receive_boat(){
    receive_rf433(boat_buf);
}

//Transmit the stored message from the uart receive through the 433MHz radio
void transmit_boat(){
    while(1){
    }

}

//Receives UART messages from the host computer 
void receive_UART(){ 
    //Index used to store where we are in the gui_buf
    int index = 0;

    //Use a single while loop to receive messages in one go.
    while(Serial.available() > 0){

        //Receive next byte from UART
        uint8_t incomingByte = Serial.read();

        //Technically we shouldn't ever reach the end of the message but this 
        //prevents buffer overflow
        if(index < MAX_MESSAGE_LENGTH - 1){
            gui_buf[index] = incomingByte;
            index++;
        }

        //If we receive a newline, it should be the end of the message
        if(incomingByte == '\n'){
            gui_buf[index] = '\0';
            return;
        }
    }

    return; //Redundant return. Maybe swap to a bool for safety.ß
}

//Transmits messages to the host computer
void transmit_UART(){
    while(1){


    }
} 



