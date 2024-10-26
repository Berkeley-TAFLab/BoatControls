/*
    This file defines all the different states 
*/


enum SM_States {
    IDLE, 
    MANUAL,
    AUTO,
    SIMULATE,
    BAD_STATE
};

enum COM_Status{
    CONNECTED,
    SEARCHING,
    BAD_COM
};


void sm_init();

void transition(SM_States next_state);

void trans_com(COM_Status next_status);

SM_States get_curr_state();

COM_Status get_com_status();