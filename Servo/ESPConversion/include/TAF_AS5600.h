/*
    This file contains all the functions that we can use 
    to read the windvane. Reference this if you need to know 
    how to use the windvane.
*/

void windvane_init(); //Setup and initialization

void read_wind_vane(); //Reads the windvane and stores in a variable
uint16_t get_cur_angle(); //Gets the last read angle from the windvane encoder
uint16_t get_avg_angle(); //Gets the averaged/denoised angle from the windvane encoder