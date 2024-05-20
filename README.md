# BoatControls
Arduino project that controls the rudder and sail based on the position of the wind vane. This document will be used in order to log current progress of the firmware.

# Devlog  

## 5/17/24
First message in the devlog, but at the moment we've converted the sail to move based on cubic polynomial. At the moment there are still some issues with the sail making excessive movements as it 
moves from 0 degrees to 360 degrees (since the encoder resets). A possible solution that I've considered is to set the "0" to the back since that excess movement is less important in the rear. 
Firmware wise, the boat should basically be ready to test 

## 5/20/24
Created branch for stepper and servo.
