void main_setup();

//Receive transmissions from boat
void receive_from_boat();

//Receives UART messages from the host computer 
void receive_from_user();

//Transmits messages to the host computer
void transmit_to_user();

void transmit_to_boat();

//Testing task in order to ensure everything works
void test_serial_task(void* parameter);