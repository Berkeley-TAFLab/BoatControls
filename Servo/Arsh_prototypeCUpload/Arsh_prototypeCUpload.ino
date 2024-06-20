
//USER LIBRARIES 

/*
  IMPORTANT PINOUTS
  Sail PWM pin - ~2 
  Tail PWM pin - ~9 Not too sure about this one 
  Radio pin 
  I2C SDA - A4
  I2C SCL - A5 


*/

#include <RH_ASK.h>
#include <Servo.h>
#include <Wire.h>
#include "AS5600.h"
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>
#endif

//USER CONSTANTS 

#define ENC_CONST_OFFSET 180; //Use this value to set where the 0 should be
#define ENC_USER_OFFSET  37;  //the encoder itself will sometimes move, use this value to correct the zeroed location.

AS5600 as5600;
Servo tailServo, sailServo;
RH_ASK driver;
unsigned long lastUpdateTime = 0;
const long updateInterval = 500; // Update interval in milliseconds
long encoderSum = 0;
int encoderCount = 0;
int serv1, serv2;
int ledPin = 8; 
int currentServoPosition = 0;

//Calibration value used to get us the true 0 position. 

void setup() {
    //Serial start for regular UART communication 
    Serial.begin(115200);

    Wire.begin();

    //For reading the encoder position
    as5600.begin(4);
    as5600.setDirection(AS5600_CLOCK_WISE);

    //Assign the tail servo and sail servo pins.
    tailServo.attach(9);
    sailServo.attach(2);

    //Assign LED pin.
    pinMode(ledPin, OUTPUT); 

    //Apparently This is for initializing all the RF stuff
    if (!driver.init()) {
        Serial.println("init failed");
    } else {
        Serial.println("RH_ASK Driver Initialized");
    }
}

void loop() {
    //Buffers used for wireless communication.
    uint8_t buf[7]; // Buffer for storing incoming data
    uint8_t buflen = sizeof(buf); // Length of the buffer

    // Try to receive data. This is only for the wireless manual controls 

    // if (driver.recv(buf, &buflen)) {
    //     digitalWrite(ledPin, HIGH); // Turn on the LED to indicate data received
    //     buf[buflen] = '\0'; // Ensure string is null-terminated to handle it as a C-style string
    //     char* received = (char*)buf;
    //     Serial.print("Received: ");
    //     Serial.println(received);
        
    //     // Parse the received string and update servo positions
    //     parseString(received, &serv1, &serv2);
    //     tailServo.write(serv1);
    //     sailServo.write(serv2);

    //     digitalWrite(ledPin, LOW); // Turn off the LED after handling data
    // }

    // Process encoder readings for wind vane angle at regular intervals
    long encoderReading = as5600.readAngle();


    encoderSum += encoderReading;
    encoderCount++;

    if (millis() - lastUpdateTime >= updateInterval) {

        if (encoderCount > 0) { // Prevent division by zero
            float averageWindVaneAngle = (float)encoderSum / encoderCount;

            //Debugging purposes
            Serial.print("Average Windvane Angle: ");
            Serial.println(averageWindVaneAngle);

            int correctedWindVaneAngle = (int) ((averageWindVaneAngle/4095) * 360) + ENC_USER_OFFSET + ENC_CONST_OFFSET; 
            if(correctedWindVaneAngle > 360){
              correctedWindVaneAngle = correctedWindVaneAngle % 360;
            }


            Serial.print("Corrected Windvane: ");
            Serial.println(correctedWindVaneAngle); 


            int servoTarget = (correctedWindVaneAngle /2 ) % 180;
            // moveServoCubic(sailServo.read(), 180-servoTarget,100, 20);

            //Print Time elapsed, average angle, and the desired angle
            Serial.print("Millis: ");
            Serial.print(millis());
            Serial.print("\t Average Angle: ");
            Serial.print(correctedWindVaneAngle);
            Serial.print("\t Servo Target: ");
            Serial.println(180 - servoTarget);

            // Reset for next average calculation
            encoderSum = 0;
            encoderCount = 0;
            lastUpdateTime = millis();
        }
       
    }

    // Minimal delay to keep loop responsive but efficient
}



//We can probably eventually just make this a PID loop
void moveServoCubic(int start, int end, int steps, int delay_time) { 
  //If within 10 degrees, do nothing.
  if(abs(start-end) <= 10){
    return;
  }
  float a = -2.0 * (end - start) / pow(steps, 3);
  float b = 3.0 * (end - start) / pow(steps, 2);
  float c = 0;
  float d = start;

  for (int i = 0; i <= steps; i++) {
    currentServoPosition = sailServo.read();

    int pos = a * pow(i, 3) + b * pow(i, 2) + c * i + d;

    sailServo.write(pos);
    delay(delay_time);
  }
}

void parseString(const char* received, int* var1, int* var2) {
    sscanf(received, "%d,%d", var1, var2);
}
