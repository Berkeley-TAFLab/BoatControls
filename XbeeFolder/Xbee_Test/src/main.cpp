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
  uint8_t payload[] = {'H', 'e', 'l', 'l', 'o'};
  transmit_xbee(payload, sizeof(payload));

  // Example of receiving data
  uint8_t receive_buffer[256];
  size_t receive_length;
  uint64_t source_address;
  uint16_t source_network_address;

  if (receive_xbee(receive_buffer, &receive_length, &source_address, &source_network_address))
  {
    Serial.print("Received from 0x");
    Serial.print(source_address, HEX);
    Serial.print(" (Network: 0x");
    Serial.print(source_network_address, HEX);
    Serial.print("): ");
    for (size_t i = 0; i < receive_length; i++)
    {
      Serial.write(receive_buffer[i]);
    }
    Serial.println();
  }

  delay(500);
}
