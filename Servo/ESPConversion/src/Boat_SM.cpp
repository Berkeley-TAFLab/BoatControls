/*
    States 
        1. Idle / standby 
        2. Manual 
        3. Autonomous 
*/

#include <Arduino.h>
//User defined libraries and headers
#include "Boat_SM.h"


static SemaphoreHandle_t state_sem; 
SM_States curr_state;

static SemaphoreHandle_t com_sem;
COM_Status com_status;


void sm_init(){
    //Should we start in idle or some steering mode 
    curr_state = IDLE; 
    com_status = SEARCHING;
    state_sem = xSemaphoreCreateMutex();
    com_sem = xSemaphoreCreateMutex();
    Serial.println("Finished initializing state machine");
}

void transition(SM_States next_state){ 
    //For now all this does is change the curr_state variable. Some more logic might need 
    //to be added later as thigns get more advanced
    if(xSemaphoreTake(state_sem, 5000) == true){
        curr_state = next_state;
        xSemaphoreGive(state_sem);
    }else{
        Serial.println("Possible deadlock on state");
    }
} 

SM_States get_curr_state(){
    SM_States return_state = BAD_STATE;
    if(xSemaphoreTake(state_sem, 5000) == true){
        return_state = curr_state;
        xSemaphoreGive(state_sem);
    }else{
        Serial.println("Possible deadlock on state");
    }
    

    return return_state;
}

COM_Status get_com_status(){
    COM_Status return_com = BAD_COM;
    if(xSemaphoreTake(com_sem, 5000) == true){
        return_com = com_status;
        xSemaphoreGive(com_sem);
    }else{
        Serial.println("Possible deadlock on com status");
    }
    

    return return_com;
}
