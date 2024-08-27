void setup_rf433(); //Setup function to initialize all necessary classes and ports for 433Mhz Radio.

bool receive_rf433(uint8_t* buf); //Function that takes messages received from 433Mhz radio and stores them into "buf"
                                

void transmit_rf433(uint8_t* buf); //Function that takes messages in a "buf" and transmits them via 433Mhz radio 

