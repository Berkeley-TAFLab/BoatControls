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
  // put your main code here, to run repeatedly:  

  receive_boat();
  receive_UART();


}

