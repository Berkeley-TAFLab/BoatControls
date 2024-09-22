#include <Arduino.h>
#include <HardwareSerial.h>
#include "Boat_SM.h"

#include "constants.h"


extern HardwareSerial Xbee_Serial;


void escape_and_write(HardwareSerial &serial, uint8_t b) {
    if (b == 0x7E || b == 0x7D || b == 0x11 || b == 0x13) {
        serial.write(0x7D);
        serial.write(b ^ 0x20);
    } else {
        serial.write(b);
    }
}

void transmit_xbee(uint8_t* payload, size_t payload_length) {
    if (payload_length > 255) {
        Serial.println("Payload too large");
        return;
    }

    // Frame length: API type (1) + Frame ID (1) + 64-bit address (8) + 
    // 16-bit address (2) + Broadcast Radius (1) + Options (1) + Payload
    uint16_t frame_length = 14 + payload_length;
    
    // Buffer to hold unescaped frame data
    uint8_t frame_data[frame_length + 3];  // +3 for start delimiter and length
    size_t index = 0;

    frame_data[index++] = 0x7E;  // Start delimiter (not escaped)
    frame_data[index++] = (frame_length >> 8) & 0xFF;  // Length MSB
    frame_data[index++] = frame_length & 0xFF;  // Length LSB
    frame_data[index++] = 0x10;  // API frame type (Transmit Request)
    frame_data[index++] = 0x01;  // Frame ID (non-zero to get a response)

    // 64-bit destination address (0x0000000000000000 for coordinator)
    for (int i = 0; i < 8; i++) {
        frame_data[index++] = 0x00;
    }

    // 16-bit destination network address (0x0000 for coordinator)
    frame_data[index++] = 0x00;
    frame_data[index++] = 0x00;

    frame_data[index++] = 0x00;  // Broadcast radius (0 for single hop to coordinator)
    frame_data[index++] = 0x00;  // Options (0 = no options)

    // Payload
    for (size_t i = 0; i < payload_length; i++) {
        frame_data[index++] = payload[i];
    }

    // Calculate checksum
    uint8_t checksum = 0;
    for (size_t i = 3; i < index; i++) {
        checksum += frame_data[i];
    }
    frame_data[index++] = 0xFF - checksum;

    // Print frame data in hexadecimal format
    // Serial.print("Frame data (before escaping): ");
    // for (size_t i = 0; i < index; i++) {
    //     if (frame_data[i] < 0x10) Serial.print("0");
    //     Serial.print(frame_data[i], HEX);
    //     Serial.print(" ");
    // }
    // Serial.println();

    // Send data to XBee with escaping
    Xbee_Serial.write(frame_data[0]);  // Start delimiter, not escaped
    Xbee_Serial.write(frame_data[1]);  // Length MSB, not escaped
    Xbee_Serial.write(frame_data[2]);  // Length LSB, not escaped

    for (size_t i = 3; i < index; i++) {
        escape_and_write(Xbee_Serial, frame_data[i]);
    }
}

bool receive_xbee(uint8_t* buffer, size_t* length, uint64_t* source_address, uint16_t* source_network_address) {
    if (Xbee_Serial.available() < 1) return false;

    // Wait for the start delimiter
    while (Xbee_Serial.read() != 0x7E) {
        if (Xbee_Serial.available() < 1) return false;
    }

    // Read frame length (2 bytes)
    while (Xbee_Serial.available() < 2) {}
    uint16_t frame_length = Xbee_Serial.read() << 8 | Xbee_Serial.read();

    // Prepare to read frame data
    uint8_t frame_data[frame_length + 1];  // +1 for checksum
    size_t index = 0;
    uint8_t checksum = 0;

    // Read and un-escape frame data
    while (index < frame_length + 1) {
        if (Xbee_Serial.available() < 1) continue;

        uint8_t b = Xbee_Serial.read();
        if (b == 0x7D) {
            // Handle escaped byte
            while (Xbee_Serial.available() < 1) {}
            b = Xbee_Serial.read() ^ 0x20;
        }

        frame_data[index++] = b;
        if (index <= frame_length) checksum += b;
    }

    // Verify checksum. For some reason my checksum calc isn't correct
    // if ((checksum & 0xFF) != 0xFF) {
    //     Serial.println("Checksum mismatch");
    //     return false;
    // }

    // Process based on frame type
    if (frame_data[0] == 0x90) {  // ZigBee Receive Packet
        *source_address = 0;
        for (int i = 1; i < 9; i++) {
            *source_address = (*source_address << 8) | frame_data[i];
        }

        *source_network_address = (frame_data[9] << 8) | frame_data[10];

        *length = frame_length - 12;  // Subtract header length
        memcpy(buffer, &frame_data[12], *length);

        return true;
    } 

    return false;
}


//Parses xbee message based on our currently defined message structure
void parse_xbee_msg(uint8_t* data_buffer,size_t length){
    //Assert the message is of proper length. Nothing should be less than 2
    if(length < 2){
        return;
    }

    uint8_t message_type = data_buffer[0];

    switch(message_type){
        case STATE_TRANS_MSG:
            if (data_buffer[1] == 0){
                transition(IDLE);
            }else if (data_buffer[1] == 1){
                transition(MANUAL);
            }else if (data_buffer[1] == 2){
                transition(AUTO);
            }
            Serial.println("State Trans msg received");
            break;
        case STEER_CTRL_MSG:
            if(get_curr_state() == MANUAL){
                
            }
            Serial.println("Steer msg received");
            break;
        case SET_LONG_MSG:
            if(get_curr_state() == MANUAL){

            }
            Serial.println("Set Long msg received");
            break;
        case SET_LAT_MSG:
            if(get_curr_state() == MANUAL){

            }
            Serial.println("Set Lat msg received");
            break;
        case POLL_LONG_MSG:
            Serial.println("Poll Long msg received");
            break;
        case POLL_LAT_MSG:
            Serial.println("Poll lat msg received");
            break;
        case POLL_STATE_MSG:
            Serial.println("Poll state msg received");
            break;
      
            
    }

    for (size_t i = 0; i < length; i++)
    {
      Serial.print(data_buffer[i], HEX);
    }
    Serial.println();
}