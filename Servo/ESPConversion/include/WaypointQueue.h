// Filename: WaypointQueue.h
// Author: Kieran Pereira
// Date: 10/11/2024
// Description: Header file for the WaypointQueue class, which provides a queue-based waypoint management system for autonomous sailboat navigation.

#ifndef WAYPOINTQUEUE_H
#define WAYPOINTQUEUE_H

#include <queue>
#include "Coordinate_Calculations.h"

class WaypointQueue {
public:
    void add_waypoint(Coordinate waypoint);
    Coordinate get_next_waypoint();
    void clear_queue();

private:
    queue<Coordinate> waypoints;
};

#endif // WAYPOINTQUEUE_H