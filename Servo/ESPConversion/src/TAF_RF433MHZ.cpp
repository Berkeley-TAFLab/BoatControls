/*
    Source file containing all functions and code related to the 433Mhz radio module
*/
//Predefined libraries and headers
#include <Arduino.h>
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

//User defined libraries and headers
#include "TAF_RF433MHZ.h"  
#include "constants.h" 
#include "Boat_SM.h"

RH_ASK driver(2000, RF_433_RX_PIN, RF_433_TX_PIN); // bitrate, rx, tx, pttpin

RHReliableDatagram manager(driver, 2); 

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN]; //You really shouldn't need this much data


/*
  THE FUNCTIONS DEFINED BELOW ARE SPECIFIC TO PARSING MESSAGES AND SHOULDN'T
  BE REFERENCED OUTSIDE OF THIS FILE.
*/


//This function is purely for debugging and ensuring the Radio module works in the first place
void test_message(uint8_t recipient){
  uint8_t data[] = "And hello back to you";

  Serial.print("got request from : 0x");
  Serial.print(recipient, HEX);
  Serial.print(": ");
  Serial.println((char*)buf);

  // Send a reply back to the control panel
  if (!manager.sendtoWait(data, sizeof(data), 1))
     Serial.println("sendtoWait failed");
}


//if message a request to set mode, parse accordingly
void parse_status_message(uint8_t buf[RH_ASK_MAX_MESSAGE_LEN]){
  if(buf[0] == SET_MODE){
    switch(buf[1]){
      case 0:
        transition(IDLE);
        break;
      case 1: 
        transition(MANUAL);
        break;
      case 2: 
        transition(AUTO);
        break;
      default: 
        Serial.println("Unrecognized Status message");
        break;
    }

  }
}

//If message is a request to adjust sail and rudder, parse accordingly
void parse_steer_message(){
  if(buf[0] == STEER_MSG){


  }
}

//If message is poll for SM status, transmit status
void poll_status(){
  if(buf[0] == POLL_STATUS){


  }
}

//If message is a poll for latitude, transmit latitude
void poll_lat(){
  if(buf[0] == POLL_LAT){


  }
}

//If message is a poll for longitude, transmit longitude
void poll_long(){
  if(buf[0] == POLL_LONG){


  }
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


void receive_rf433()
{
  Serial.println("Attempting to Receive");
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t recipient;
    if (manager.recvfromAck(buf, &len, &recipient))
    {
      //debug message to test if radio module works.
      test_message(recipient); 

    }
  }
} 