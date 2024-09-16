#include <stdint.h> 
#include <stddef.h>


//TODO: ADD AN RX MESSAGE STRUCT 

//TODO: Someone is probably better at xbee than me i'll leave some dev room
void setup_xbee();

//Transmit data via uart to the coordinator
void transmit_xbee(uint8_t* payload, size_t payload_length);  

//Receive incoming data from the xbee
bool receive_xbee(uint8_t* buffer, size_t* length, uint64_t* source_address, uint16_t* source_network_address); 