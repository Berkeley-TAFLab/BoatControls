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
    void initialize_autonomous_mode();
    Datatypes::Coordinate get_next_waypoint();
    void add_waypoint(Datatypes::Coordinate waypoint);
    void clear_queue();
    bool is_empty() const;
    void execute_waypoints();
    void close_autonomous_mode();

private:
    queue<Datatypes::Coordinate> waypoints;
    mutex queue_mutex;
    condition_variable queue_condition;
    bool autonomous_mode = false;
    bool running = false;
    thread execution_thread;
    Datatypes::Coordinate previous_waypoint;
};

#endif // WAYPOINTQUEUE_HPP