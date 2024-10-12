// Filename: Coordinate_Calculations.h
// Author: Kieran Pereira
// Last Modified: 10/11/2024
// Description: A script containing all relevant functions to do coordinate calculations (including tacking path planning)

#ifndef TACKING_PATH_PLANNER_H
#define TACKING_PATH_PLANNER_H

#include <vector>

using namespace std;

// Defining Coordinate Struct
struct Coordinate {
    float latitude;
    float longitude;
};

// Class Definition
class TackingPathPlanner {
public:
    TackingPathPlanner(float wind_direction);


    float calculate_optimal_sail_angle(float wind_direction);
    float calculate_distance(Coordinate coord1, Coordinate coord2);
    float calculate_bearing(Coordinate start, Coordinate end);
    vector<Coordinate> plan_tack_path(Coordinate start, Coordinate destination);

};

#endif // TACKING_PATH_PLANNER_H