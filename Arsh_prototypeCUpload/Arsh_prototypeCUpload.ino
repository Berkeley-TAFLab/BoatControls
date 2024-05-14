#include <RH_ASK.h>
#include <Servo.h>
#include <Wire.h>
#include "AS5600.h"
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>
#endif

AS5600 as5600;
Servo myServo, tailServo, sailServo;
RH_ASK driver;
unsigned long lastUpdateTime = 0;
const long updateInterval = 2000; // Update interval in milliseconds
long encoderSum = 0;
int encoderCount = 0;
int serv1, serv2;
int ledPin = 8; 
int currentServoPosition = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    as5600.begin(4);
    as5600.setDirection(AS5600_CLOCK_WISE);
    myServo.attach(2);
    tailServo.attach(2);
    sailServo.attach(9);
    pinMode(ledPin, OUTPUT);

    if (!driver.init()) {
        Serial.println("init failed");
    } else {
        Serial.println("RH_ASK Driver Initialized");
    }
}

void loop() {
    uint8_t buf[7]; // Buffer for storing incoming data
    uint8_t buflen = sizeof(buf); // Length of the buffer

    // Try to receive data
    if (driver.recv(buf, &buflen)) {
        digitalWrite(ledPin, HIGH); // Turn on the LED to indicate data received
        buf[buflen] = '\0'; // Ensure string is null-terminated to handle it as a C-style string
        char* received = (char*)buf;
        Serial.print("Received: ");
        Serial.println(received);
        
        // Parse the received string and update servo positions
        parseString(received, &serv1, &serv2);
        tailServo.write(serv1);
        sailServo.write(serv2);

        digitalWrite(ledPin, LOW); // Turn off the LED after handling data
    }

    // Process encoder readings for wind vane angle at regular intervals
    encoderSum += as5600.readAngle();
    encoderCount++;

    if (millis() - lastUpdateTime >= updateInterval) {
        if (encoderCount > 0) { // Prevent division by zero
            float averageWindVaneAngle = (float)encoderSum / encoderCount;
            int correctedWindVaneAngle = map(averageWindVaneAngle, 0, 4095, 0, 360);
            int servoTarget = map(correctedWindVaneAngle, 0, 360, 180, 5);

            if (abs(servoTarget - currentServoPosition) > 5) {
                myServo.write(servoTarget);
                currentServoPosition = servoTarget; // Update the servo position

                Serial.print("Millis: ");
                Serial.print(millis());
                Serial.print("\t Average Angle: ");
                Serial.print(correctedWindVaneAngle);
                Serial.print("\t Servo Target: ");
                Serial.println(servoTarget);
            }

            // Reset for next average calculation
            encoderSum = 0;
            encoderCount = 0;
            lastUpdateTime = millis();
        }
    }

    // Minimal delay to keep loop responsive but efficient
    delay(100);
}


void parseString(const char* received, int* var1, int* var2) {
    sscanf(received, "%d,%d", var1, var2);
}
