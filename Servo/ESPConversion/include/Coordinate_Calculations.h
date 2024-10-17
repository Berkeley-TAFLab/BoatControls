// Filename: Coordinate_Calculations.h
// Author: Kieran Pereira
// Last Modified: 10/11/2024
// Description: A script containing all relevant functions to do coordinate calculations (including tacking path planning)

#ifndef TACKING_PATH_PLANNER_H
#define TACKING_PATH_PLANNER_H

#include <vector>
#include "DataTypes.hpp"

using namespace std;

// Class Definition
class CoordinateCalcuations {
public:
    float calculate_optimal_sail_angle(float wind_direction);
    Datatypes::Coordinate convert_to_position(Datatypes::Coordinate position, float angle, float distance);
    float calculate_distance(Datatypes::Coordinate coord1, Datatypes::Coordinate coord2);
    float calculate_bearing(Datatypes::Coordinate start, Datatypes::Coordinate end);
    std::pair<std::string, float> CoordinateCalcuations::calculate_directional_bearing(Datatypes::Coordinate target_waypoint);
    vector<Datatypes::Coordinate> plan_tack_path(Datatypes::Coordinate start, Datatypes::Coordinate destination);
    
private:
    static const unsigned int max_upwind_angle = 45;
};

#endif // TACKING_PATH_PLANNER_H