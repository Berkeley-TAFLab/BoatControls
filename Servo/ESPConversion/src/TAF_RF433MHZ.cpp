/*
    Source file containing all functions and code related to the 433Mhz radio module
*/
//Predefined libraries and headers
#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

//User defined libraries and headers
#include "TAF_RF433MHZ.h"  
#include "constants.h" 

RH_ASK driver(2000, RF_433_RX_PIN, RF_433_TX_PIN, -1); // bitrate, rx, tx, pttpin

RHReliableDatagram manager(driver, RF_ID); 

void setup_rf433(){
    if(!manager.init()){
        Serial.println("Failed to init RF manager class.");
    }
} 

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

void receive_rf433()
{
  if (manager.available())
  {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (manager.recvfromAck(buf, &len, &from))
    {
      Serial.print("got request from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);

      // Send a reply back to the originator client
      if (!manager.sendtoWait(data, sizeof(data), from))
        Serial.println("sendtoWait failed");
    }
  }
}
