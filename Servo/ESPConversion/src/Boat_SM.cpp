/*
    States 
        1. Idle / standby 
        2. Manual 
        3. Autonomous 
*/


//User defined libraries and headers
#include "Boat_SM.h"


//This variable is most likely going to need some sort of lock on it .
SM_States curr_state;


void sm_init(){
    //Should we start in idle or some steering mode 
    curr_state = IDLE; 
}

void transition(SM_States next_state){ 
    //For now all this does is change the curr_state variable. Some more logic might need 
    //to be added later as thigns get more advanced
    curr_state = next_state;
} 

SM_States get_curr_state(){
    return curr_state;
}
