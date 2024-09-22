#include <stdint.h> 
#include <stddef.h>

//Transmit data via uart to the coordinator
void transmit_xbee(uint8_t* payload, size_t payload_length);  

//Receive incoming data from the xbee
bool receive_xbee(uint8_t* buffer, size_t* length, uint64_t* source_address, uint16_t* source_network_address); 

//Function to parse messages by ID field
void parse_xbee_msg(uint8_t* data_buffer,size_t length);