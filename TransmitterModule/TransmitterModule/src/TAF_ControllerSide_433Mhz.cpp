/*
    Source file containing all functions and code related to the 433Mhz radio module
*/
//Predefined libraries and headers
#include <Arduino.h>
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

//User defined libraries and headers
#include "TAF_ControllerSide_433Mhz.h" 
#include "TAF_GUI_UARTComm.h" //Include to pass through data to buffer
#include "constants.h" 

RH_ASK driver(2000, RF_433_RX_PIN, RF_433_TX_PIN); // bitrate, rx, tx, pttpin

//Second parameter is the ID of the boat. If set incorrectly, it will not receive the message
RHReliableDatagram manager(driver, 2); 



/*
  THE FUNCTIONS DEFINED BELOW ARE SPECIFIC TO PARSING MESSAGES AND SHOULDN'T
  BE REFERENCED OUTSIDE OF THIS FILE.
*/
void store_message(){
    //This just stores the message in order for us to use it later
}



/*
  THE FUNCTIONS DEFINED BELOW ARE USED IN RECEIVING MESSAGES AND SETITNG UP 
  THE 433MHZ RADIO MODULES. THEY MAY BE REFERENCED OUTSIDE OF THIS FILE
*/


void setup_rf433(){

    if(!manager.init()){
        Serial.println("Failed to init RF manager class.");
    }else{
      Serial.println("Initialized RF Manager");
    }
} 


void receive_rf433(uint8_t* buf)
{
  //Debugging message to see if we enter the receive function or not 
  // Serial.println("Attempting to Receive");
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t recipient;
    if (manager.recvfromAck(buf, &len, &recipient))
    { 
      Serial.print("Received From: ");
      Serial.println(recipient,HEX);


    }
  }
} 

void transmit_uart(){

}
