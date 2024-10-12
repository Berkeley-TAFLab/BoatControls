// Filename: WaypointQueue.cpp
// Author: Kieran Pereira
// Date: 10/11/2024
// Description: Implementation file for the WaypointQueue class, providing a queue-based waypoint management system.

#include "WaypointQueue.h"

// Adds a waypoint to the queue
void WaypointQueue::add_waypoint(Coordinate waypoint) {
    waypoints.push(waypoint);
}

// Retrieves and removes the next waypoint from the queue
Coordinate WaypointQueue::get_next_waypoint() {
    if (!waypoints.empty()) {
        Coordinate next = waypoints.front();
        waypoints.pop();
        return next;
    } else {
        return {0.0, 0.0}; // Placeholder for empty queue
    }
}

// Clears all waypoints from the queue (Emergency override)
void WaypointQueue::clear_queue() {
    while (!waypoints.empty()) {
        waypoints.pop();
    }
}