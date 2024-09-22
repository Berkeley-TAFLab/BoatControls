/*
    This file defines all the different states 
*/


enum SM_States {
    IDLE, 
    MANUAL,
    AUTO
};

enum COM_Status{
    CONNECTED,
    SEARCHING
};


void sm_init();

void transition(SM_States next_state);

SM_States get_curr_state();

COM_Status get_com_status();