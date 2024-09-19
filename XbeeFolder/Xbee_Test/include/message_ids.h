// This text document will be used to record the message IDs of every message being sent 
// on the boat 

// Control Panel to boat 
// 	0x01 - State Transition 
// 	0x02 - Steer Controls
// 	0x03 - Set Longitude 
// 	0x04 - Set Latitude
// 	0x05 - Poll Longitude 
// 	0x06 - Poll Latitude 
// 	0x07 - Poll State 

// Boat to Control Panel 
// 	0x08 - Send Status 
// 	0x09 - Send Longitude 
// 	0x0A - Send Latitude 
// 	0x0B - Send Heading 

#define STATE_TRANS_MSG 0x01 
#define STEER_CTRL_MSG 0X02 
#define SET_LONG_MSG 0x03 
#define SET_LAT_MSG 0x04 
#define POLL_LONG_MSG 0x05 
#define POLL_LAT_MSG 0x06 
#define POLL_STATE_MSG 0x07 

#define SEND_STATUS_MSG 0x08
#define SEND_LONG_MSG 0X09
#define SEND_LAT_MSG 0x0A
#define SEND_HEAD_MSG 0x0B