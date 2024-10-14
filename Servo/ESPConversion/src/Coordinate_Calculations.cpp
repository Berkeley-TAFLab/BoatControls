// Filename: Coordinate_Calculations.cpp
// Author: Kieran Pereira
// Last Modified: 10/11/2024
// Description: A script containing all relevant functions to do coordinate calculations (including tacking path planning)

#include <iostream>
#include <cmath>
#include <vector>
#include "Coordinate_Calculations.h"
#include "Boat_steer.h"
#include "TAF_AS5600.h" 
#include "WaypointQueue.hpp"

using namespace std;

const unsigned int EARTH_RADIUS = 6371000.0; // Earth radius in meters
const float DEG_TO_RAD = (M_PI / 180.0f);
const float RAD_TO_DEG = (180.0f / M_PI);

float CoordinateCalcuations::calculate_optimal_sail_angle(float wind_direction)
{
    if (wind_direction < 30)
    {
        // Creating logic for getting out of irons
        set_rudder_servo(70);
        set_sail_servo(70);
        return 70; // Example return to indicate action taken
    }
    else if (wind_direction < 45)
    {
        return 20;
    }
    else if (wind_direction > 135)
    {
        return 90;  // Downwind, wide sail angle (around 90 degrees)
    }
    else
    {
        return wind_direction / 2;  // Beam reach, moderate sail angle
    }
}

// Calculating distance between two coordinate points using the Haversine Formula
float CoordinateCalcuations::calculate_distance(Datatypes::Coordinate coord1, Datatypes::Coordinate coord2) {
    float delta_lat = (coord2.latitude - coord1.latitude) * DEG_TO_RAD;
    float delta_lon = (coord2.longitude - coord1.longitude) * DEG_TO_RAD;
    float a = sin(delta_lat / 2) * sin(delta_lat / 2) +
              cos(coord1.latitude * DEG_TO_RAD) * cos(coord2.latitude * DEG_TO_RAD) *
              sin(delta_lon / 2) * sin(delta_lon / 2);

    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}


float CoordinateCalcuations::calculate_bearing(Datatypes::Coordinate curr_coord, Datatypes::Coordinate tar_coord)
{
    // Converting to radians
    float curr_lat_rad = curr_coord.latitude * DEG_TO_RAD;
    float curr_lon_rad = curr_coord.longitude * DEG_TO_RAD;
    float tar_lat_rad = tar_coord.latitude * DEG_TO_RAD;
    float tar_lon_rad = tar_coord.longitude * DEG_TO_RAD;

    // Calculate the difference in longitudes
    float longitude_distance = tar_lon_rad - curr_lon_rad;

    // Bearing formula obtained from https://www.igismap.com/formula-to-find-bearing-or-heading-angle-between-two-points-latitude-longitude/
    float y = sin(longitude_distance) * cos(tar_lat_rad);
    float x = cos(curr_lat_rad) * sin(tar_lat_rad) - sin(curr_lat_rad) * cos(tar_lat_rad) * cos(longitude_distance);
    float bearing = atan2(y, x);

    bearing = bearing * RAD_TO_DEG;
    bearing = fmod((bearing + 360), 360);

    return bearing;
}

Datatypes::Coordinate CoordinateCalcuations::convert_to_position(Datatypes::Coordinate position, float angle, float distance)

{

    // Convert latitude and longitude from degrees to radians
    float lat_rad = position.latitude * DEG_TO_RAD;
    float lon_rad = position.longitude * DEG_TO_RAD;
    float angle_rad = angle * DEG_TO_RAD;

    // Calculate the new latitude
    float new_lat_rad = asin(sin(lat_rad) * cos(distance / EARTH_RADIUS) + cos(lat_rad) * sin(distance / EARTH_RADIUS) * cos(angle_rad));

    // Calculate the new longitude
    float new_lon_rad = lon_rad + atan2(sin(angle_rad) * sin(distance / EARTH_RADIUS) * cos(lat_rad), cos(distance / EARTH_RADIUS) - sin(lat_rad) * sin(new_lat_rad));

    // Convert the new latitude and longitude from radians to degrees
    float new_lat = new_lat_rad * RAD_TO_DEG;
    float new_long = new_lon_rad * RAD_TO_DEG;

    Datatypes::Coordinate new_position = {new_lat, new_long};
    // Return the new position as a Coordinate
    return new_position;
}


vector<Datatypes::Coordinate> CoordinateCalcuations::plan_tack_path(Datatypes::Coordinate start, Datatypes::Coordinate destination) 
{
    WaypointQueue waypointQueue;
    vector<Datatypes::Coordinate> waypoints;
    Datatypes::Coordinate current_position = start;
    float wind_direction = get_avg_angle();

    // Calculate initial bearing to destination
    float destination_bearing = calculate_bearing(current_position, destination);

    // Calculate the angle difference between the desired heading and wind direction
    float angle_to_wind = abs(wind_direction - destination_bearing);
    if (angle_to_wind > 180) 
    {
        angle_to_wind = 360 - angle_to_wind;  // Normalize to 0-180 degrees
    }

    // Defining no go zone relative to wind directiion
    // TODO: Understand if wind sensor is taken relative to north or relative to boat
    // TODO: Needs to be unit tested

    // If the destination is upwind, plan a tacking path
    if (angle_to_wind <= max_upwind_angle)
    {
        float straight_distance = calculate_distance(current_position, destination);
        float tack_diagonal_distance = straight_distance / cos(max_upwind_angle * 3.14 / 180.0);

        // Determine if the boat is moving left or right relative to the wind direction
        float bearing_difference = wind_direction - destination_bearing;
        if (bearing_difference < 0) 
        {
            bearing_difference += 360;
        }

        // If moving right of the wind direction (0-180 degrees), tack right
        if (bearing_difference < 180) 
        {
            waypointQueue.add_waypoint(convert_to_position(current_position,bearing_difference, tack_diagonal_distance));
        }
        // If moving left of the wind direction (180-360 degrees), tack left
        else {
            waypointQueue.add_waypoint(convert_to_position(current_position,bearing_difference+180, tack_diagonal_distance));
        }
    }

    // Add final waypoint to reach the destination
    while (!waypointQueue.is_empty()) {
    waypoints.push_back(waypointQueue.get_next_waypoint());
    }
    waypoints.push_back(destination);
    return waypoints;