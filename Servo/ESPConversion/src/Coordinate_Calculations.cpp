// Filename: Coordinate_Calculations.cpp
// Author: Kieran Pereira
// Last Modified: 10/11/2024
// Description: A script containing all relevant functions to do coordinate calculations (including tacking path planning)

#include <iostream>
#include <cmath>
#include <vector>
#include "Coordinate_Calculations.h"
#include "Boat_steer.h"

using namespace std;

float TackingPathPlanner::calculate_optimal_sail_angle(float wind_direction)
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

// Calculating distance between two coordinate points
float TackingPathPlanner::calculate_distance(Coordinate coord1, Coordinate coord2) {
    // Using a simple approximation for distance calculation (Haversine or a similar method would be more precise)
    float delta_lat = coord2.latitude - coord1.latitude;
    float delta_lon = coord2.longitude - coord1.longitude;
    return sqrt(delta_lat * delta_lat + delta_lon * delta_lon);
}

float TackingPathPlanner::calculate_bearing(Coordinate curr_coord, Coordinate tar_coord)
{
    // Converting to radians
    float curr_lat_rad = radians(curr_coord.latitude);
    float curr_lon_rad = radians(curr_coord.longitude);
    float tar_lat_rad = radians(tar_coord.latitude);
    float tar_lon_rad = radians(tar_coord.longitude);

    // Calculate the difference in longitudes
    float longitude_distance = tar_lon_rad - curr_lon_rad;

    // Bearing formula obtained from https://www.igismap.com/formula-to-find-bearing-or-heading-angle-between-two-points-latitude-longitude/
    float y = sin(longitude_distance) * cos(tar_lat_rad);
    float x = cos(curr_lat_rad) * sin(tar_lat_rad) - sin(curr_lat_rad) * cos(tar_lat_rad) * cos(longitude_distance);
    float bearing = atan2(y, x);

    // Convert bearing from radians to degrees
    bearing = bearing * 180.0 / PI;

    // Normalize to 0-360 degrees
    if (bearing < 0)
    {
        bearing += 360;
    }
    return bearing;
}

vector<Coordinate> TackingPathPlanner::plan_tack_path(Coordinate start, Coordinate destination) {
    vector<Coordinate> waypoints;
    Coordinate current_position = start;

    // Calculate initial bearing to destination
    float destination_bearing = calculate_bearing(current_position, destination);

    // Calculate the angle difference between the desired heading and wind direction
    float angle_to_wind = abs(wind_direction - destination_bearing);
    if (angle_to_wind > 180) {
        angle_to_wind = 360 - angle_to_wind;  // Normalize to 0-180 degrees
    }

    // If the destination is upwind, plan a tacking path
    if (angle_to_wind <= 45) {
        float straight_distance = calculate_distance(current_position, destination);
        static const unsigned int max_upwind_angle = 45;
        float tack_diagonal_distance = straight_distance / cos(max_upwind_angle * PI / 180.0);

        // Determine if the boat is moving left or right relative to the wind direction
        float bearing_difference = wind_direction - destination_bearing;
        if (bearing_difference < 0) {
            bearing_difference += 360;
        }

        // If moving right of the wind direction (0-180 degrees), tack right
        if (bearing_difference < 180) {
            Coordinate waypoint = add_waypoint(current_position, wind_direction + 45, tack_diagonal_distance);
            waypoints.push_back(waypoint);
        }
        // If moving left of the wind direction (180-360 degrees), tack left
        else {
            Coordinate waypoint = add_waypoint(current_position, wind_direction - 45, tack_diagonal_distance);
            waypoints.push_back(waypoint);
        }
    }

    // Add final waypoint to reach the destination
    waypoints.push_back(destination);
    return waypoints;
}