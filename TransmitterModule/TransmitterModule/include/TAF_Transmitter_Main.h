void main_setup();


//Receives UART messages from the host computer 
void receive_UART();

//Transmits messages to the host computer
void transmit_UART();

void transmit_boat();

//Testing task in order to ensure everything works
void test_serial_task(void* parameter);