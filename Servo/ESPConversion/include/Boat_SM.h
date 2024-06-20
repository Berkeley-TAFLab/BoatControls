enum SM_States {
    IDLE, 
    MANUAL,
    AUTO
};


void sm_init();

void transition(SM_States next_state);

SM_States get_curr_state();