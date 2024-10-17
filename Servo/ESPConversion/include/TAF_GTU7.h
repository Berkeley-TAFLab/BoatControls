// Filename: Coordinate_Calculations.h
// Author: Kieran Pereira
// Last Modified: 10/11/2024
// Description: The header file for TAF_GTU7.cpp for interacting with the GPS module

#ifndef GTU7_MODULE_HPP
#define GTU7_MODULE_HPP

#include "Datatypes.hpp"
#include <cstdint> // For uint8_t

// Setup necessary semaphores and other initialization for the GPS
void setup_gtu7();

// Read UART transmission from GTU7 module
void read_gtu7();

// Check if the most recent data is valid; strongly suggest using this before accessing data
bool valid_data();

// Retrieve stored latitude data from GTU7 module
float get_gtu7_lat();

// Retrieve stored longitude data from GTU7 module
float get_gtu7_long();

// Retrieve stored altitude data in meters from GTU7 module
float get_gtu7_alt();

// Compress latitude data for transmission
void compress_lat(float latitude, uint8_t* output);

// Compress longitude data for transmission
void compress_long(float longitude, uint8_t* output);

// Get the current coordinate as a Datatypes::Coordinate
Datatypes::Coordinate get_curr_coordinate();

#endif // GTU7_MODULE_HPP
