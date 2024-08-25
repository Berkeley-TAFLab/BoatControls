
//Predefined libraries and headers
#include <Arduino.h>
#include <TinyGPSPlus.h>

//User defined libraries and headers
#include "TAF_GTU7.h" 

//User defined private variables
TinyGPSPlus gps;

extern HardwareSerial Gps_Serial; 

//Add a single mutex for these values
float gtu7_lat = 0; 
float gtu7_long = 0; 
float gtu7_alt = 0; 

bool updated_val = false;

void read_gtu7(){
    updated_val = false;
    //Consider adding a timeout so we aren't held up by this value
    while(Gps_Serial.available()>0){
        char c = Gps_Serial.read();
        gps.encode(c);
    } 

    //Update for more variables if necessary 
    if(gps.location.isUpdated()){
        gtu7_lat = gps.location.lat();
        gtu7_long = gps.location.lng();
        gtu7_alt = gps.altitude.meters();
        updated_val = true;
    }

    if (gps.location.isValid())
    {
        Serial.print(gps.location.lat(), 6);
        Serial.print(F(","));
        Serial.print(gps.location.lng(), 6);
    }
}

float get_gtu7_lat(){
    return gtu7_lat;
}

float get_gtu7_long(){
    return gtu7_long;
}

float get_gtu7_alt(){
    return gtu7_alt;
}

bool valid_data(){
    return updated_val;
}


//Potential fields you can consider adding in later

/*    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    Serial.print("Date: ");
    Serial.print(gps.date.month());
    Serial.print("/");
    Serial.print(gps.date.day());
    Serial.print("/");
    Serial.println(gps.date.year());
    Serial.print("Time: ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.println(gps.time.second());
    Serial.println();*/

