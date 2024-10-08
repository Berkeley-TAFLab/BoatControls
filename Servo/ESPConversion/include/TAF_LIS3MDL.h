
void setup_lis3mdl(); // Setup function for magnetometer 


//if we use semaphores will it be better to have a semasetup 
//and a device setup? 

float get_heading_lis3mdl(); // Calculate heading based on read values

void read_lis3mdl(); // Call to start a reading 