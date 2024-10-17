#include <cstdint>

//Set 
void set_sail_servo(uint8_t position);

void set_rudder_servo(uint8_t position); 

void setup_steer();

uint8_t get_sail_position();

uint8_t get_rudder_position();


void manual_steer();

void steering_boat(float bearing);

void auto_steer();

