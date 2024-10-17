
//Predefined libraries and headers
#include <Arduino.h>
#include <TinyGPSPlus.h>

//User defined libraries and headers
#include "TAF_GTU7.h" 
#include "DataTypes.hpp"

//User defined private variables
TinyGPSPlus gps;

extern HardwareSerial Gps_Serial; 

//Add a single mutex for these values
static SemaphoreHandle_t lat_sem; 
float gtu7_lat = 0; 

static SemaphoreHandle_t long_sem; 
float gtu7_long = 0; 

float gtu7_alt = 0; 

bool updated_val = false;

void setup_gtu7(){
    lat_sem = xSemaphoreCreateMutex();
    long_sem = xSemaphoreCreateMutex();
}

void read_gtu7(){
    updated_val = false;
    //Consider adding a timeout so we aren't held up by this value
    while(Gps_Serial.available()>0){
        char c = Gps_Serial.read();
        gps.encode(c);
    } 

    //Update for more variables if necessary 
    if(gps.location.isUpdated()){

        if(xSemaphoreTake(lat_sem, 5000) == true){
            gtu7_lat = gps.location.lat();
            xSemaphoreGive(lat_sem);
        }else{
            Serial.println("Possible deadlock on latitude");
        }


        if(xSemaphoreTake(long_sem, 5000) == true){
            gtu7_long = gps.location.lng();
            xSemaphoreGive(long_sem);
        }else{
            Serial.println("Possible deadlock on longitude");
        }
        gtu7_alt = gps.altitude.meters();
        updated_val = true;
    }

    // if (gps.location.isValid())
    // {
    //     Serial.print(gps.location.lat(), 6);
    //     Serial.print(F(","));
    //     Serial.print(gps.location.lng(), 6);
    // }
}

float get_gtu7_lat(){
    float curr_lat = -999.0; //Initialize to an error value
    if(xSemaphoreTake(lat_sem, 5000) == true){
        curr_lat = gtu7_lat;
        xSemaphoreGive(lat_sem);
    }else{
        Serial.println("Possible deadlock on latitude");
    }
    return curr_lat;
}

Datatypes::Coordinate get_curr_coordinate()
{
    return {get_gtu7_lat(),get_gtu7_long()};
}

float get_gtu7_long(){
    float curr_long = -999.0; 
    if(xSemaphoreTake(long_sem, 5000) == true){
        curr_long = gtu7_long;
        xSemaphoreGive(long_sem);
    }else{
        Serial.println("Possible deadlock on longitude");
    }    
    return curr_long;
}

float get_gtu7_alt(){
    return gtu7_alt;
}

bool valid_data(){
    return updated_val;
}

void compress_lat(float latitude, uint8_t* output){
    int32_t scaled_latitude = (int32_t)(latitude * 100000); 

    output[0] = (scaled_latitude >> 24) & 0xFF; // Most significant byte
    output[1] = (scaled_latitude >> 16) & 0xFF;
    output[2] = (scaled_latitude >> 8) & 0xFF;
    output[3] = scaled_latitude & 0xFF; // Least significant byte    
}

void compress_long(float longitude, uint8_t* output){
    int32_t scaled_longitude = (int32_t)(longitude * 100000); 
    
    output[0] = (scaled_longitude >> 24) & 0xFF; // Most significant byte
    output[1] = (scaled_longitude >> 16) & 0xFF;
    output[2] = (scaled_longitude >> 8) & 0xFF;
    output[3] = scaled_longitude & 0xFF; // Least significant byte    
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

