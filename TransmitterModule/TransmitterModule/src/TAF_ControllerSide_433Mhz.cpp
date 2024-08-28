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
#include "constants.h" 

RH_ASK driver(5000, RF_433_RX_PIN, RF_433_TX_PIN); // bitrate, rx, tx, pttpin

//Second parameter is the ID of the boat. If set incorrectly, it will not receive the message
RHReliableDatagram manager(driver, 2); 


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

//Receive message via radio module
bool receive_rf433(uint8_t* buf)
{
  //Debugging message to see if we enter the receive function or not 
  // Serial.println("Attempting to Receive");
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = MAX_MESSAGE_LENGTH;
    uint8_t recipient;
    if (manager.recvfromAck(buf, &len, &recipient))
    { 
      return true;
    }
  }

  return false;
}  

//Transmit a message through the radio module
void transmit_rf433(uint8_t* buf){

  uint8_t from = buf[0];
  uint8_t* trans_data = buf + 1;
  if (!manager.sendtoWait(trans_data, strlen((char*)trans_data), from))
    Serial.println("sendtoWait failed");

  //Unimplemented for now
  // Serial.print("Data received: ");
  // for (size_t i = 0; i < 8; i++) {
  //   Serial.print(buf[i], HEX);  // Print each byte in hexadecimal format
  //   Serial.print(" ");          // Add a space between bytes
  // }
  // Serial.println();  // Print a new line after printing all bytes

} 