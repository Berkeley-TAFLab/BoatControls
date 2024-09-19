#include <Arduino.h>
#include "xbee_msg.h"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  setup_xbee();
  delay(1000);
  Serial.println("Completed Setup.");
}

void loop()
{
  // put your main code here, to run repeatedly:
  // uint8_t payload[] = {'H', 'e', 'l', 'l', 'o'};
  // transmit_xbee(payload, sizeof(payload));

  // Example of receiving data
  uint8_t receive_buffer[256];
  size_t receive_length;
  uint64_t source_address;
  uint16_t source_network_address;

  if (receive_xbee(receive_buffer, &receive_length, &source_address, &source_network_address))
  {
    Serial.println("received messagefg");
    parse_xbee_msg(receive_buffer, receive_length);


  }

  delay(500);
}
