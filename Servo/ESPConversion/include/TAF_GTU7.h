
void setup_gtu7(); //Setup necessary semaphores and other things for the gps

void read_gtu7(); //Read UART transmission from gtu7 module 

bool valid_data(); // Is the most recent data valid. Strongly suggest using this before accessing data

float get_gtu7_lat(); //Retrieve stored latitude data from gtu7 module

float get_gtu7_long(); // Retrieve stored longitude data from gtu7 module

float get_gtu7_alt(); //Retrieve stored altitude data in meters from gtu7 module

void compress_lat(float latitude, uint8_t* output);

void compress_long(float longitude, uint8_t* output);