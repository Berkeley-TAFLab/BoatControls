// Filename: WaypointQueue.hpp
// Author: Kieran Pereira
// Date: 10/11/2024
// Description: Header file for the WaypointQueue class, providing a queue-based waypoint management system with autonomous mode handling.

#ifndef WAYPOINTQUEUE_HPP
#define WAYPOINTQUEUE_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Datatypes.hpp"

using namespace std;

class WaypointQueue {
public:
    void add_waypoint(Datatypes::Coordinate waypoint);
    Datatypes::Coordinate get_next_waypoint();
    bool is_empty() const;
    void clear_queue();
    void initialize_autonomous_mode();
    void execute_waypoints();

private:
    queue<Datatypes::Coordinate> waypoints;
    mutex queue_mutex;
    condition_variable queue_condition;
    bool autonomous_mode = false;
    bool running = false;
};

#endif // WAYPOINTQUEUE_HPP
