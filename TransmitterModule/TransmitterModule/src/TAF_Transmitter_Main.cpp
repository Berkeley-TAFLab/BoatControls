//Predefined libraries and headers
#include <Arduino.h> 

//User defined libraries and headers
#include "TAF_Transmitter_Main.h" 
#include "TAF_ControllerSide_433Mhz.h" //Header file with functions related to the 433MHz radios.
#include "constants.h"  //Header file with constant variables used for device configuration.

//User defined private variables
uint8_t boat_buf[MAX_MESSAGE_LENGTH]; //Buffer used to store messages from the boats
uint8_t gui_buf[MAX_MESSAGE_LENGTH]; //Buffer used to store messages from the GUI

bool boat_receive_flag = false; //Flag that indicates if we have a message from a boat
bool uart_receive_flag = false; //Flag that indicates if we have a message from the user

void main_setup(){
    Serial.begin(BAUD_RATE);

    //Call to setup the manager class for 433MHz radio
    setup_rf433();

} 

//Since you can't transmit and receive wirelessly at the same time a mutex or semaphore should be used

//Receive from the 433MHz radio module and store value 
void receive_from_boat(){
    boat_receive_flag = receive_rf433(boat_buf);
}

//Transmit the stored message from the uart receive through the 433MHz radio
void transmit_to_boat(){
    if(uart_receive_flag){
        //Transmit data stored in the gui_buf
        transmit_rf433(gui_buf);
    }    

}

//Receives UART messages from the host computer 
void receive_from_user(){ 
    uart_receive_flag = false;  // Reset the flag initially

    // Read the incoming message until a newline character is received
    String input = Serial.readStringUntil('\n');

    // Check if the input is non-empty and does not exceed buffer size
    if (input.length() > 0 && input.length() < MAX_MESSAGE_LENGTH) {
        // Copy the input message to the gui_buf
        input.getBytes(gui_buf, MAX_MESSAGE_LENGTH);

        uart_receive_flag = true;  // Set flag indicating a message has been received

        // Debugging output to verify the received message
        Serial.print("Received from user: ");
        Serial.println(input);  // Print the received message as a string
    }
}

//Transmits messages to the host computer via UART
void transmit_to_user(){   
    if(boat_receive_flag){
        for (int i = 0; i < MAX_MESSAGE_LENGTH; i++) {
            // Stop printing if we reach the end of the message (null character)
            if (boat_buf[i] == '\0' || boat_buf[i] == '\n') {
                break;
            }
            
            Serial.print(boat_buf[i], HEX);  // Print each byte in hexadecimal format
        }
    }
} 



