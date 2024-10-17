#include <cstdint>

//Set 
void set_sail_servo(uint8_t position);

void set_rudder_servo(uint8_t position); 

void setup_steer();

uint8_t get_sail_position();

uint8_t get_rudder_position();


void manual_steer();
void auto_steer();

// @brief Calculates bearing given inputs of current lat/ long and target lat/ long
// @param curr_lat boats current latitude
// @param curr_long boats current longitude
// @param tar_lat boats target latitude
// @param tar_long boats target longitude
float calculate_bearing(float curr_lat, float curr_long, float tar_lat, float tar_long);