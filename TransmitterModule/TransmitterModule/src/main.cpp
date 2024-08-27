#include "main.h"

void setup() {
  // put your setup code here, to run once:
  main_setup(); 



}

/*
  Since the transmitter simply operates as a passthrough the general process 
  of the pseduocode follows the following sequence:

  1. Receive transmissions from boat
  2. Receive UART transmissions from GUI 
  3. Send boat transmissions to GUI 
  4. Send GUI requests to boats.
*/

void loop() {

  // These functions represent step 1 and 2 
  receive_from_boat();
  receive_from_user(); 

  //These function represent steps 3 and 4
  transmit_to_boat();
  transmit_to_user();


}

