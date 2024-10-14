// Filename: WaypointQueue.cpp
// Author: Kieran Pereira
// Date: 10/11/2024
// Description: Implementation file for the WaypointQueue class, providing a queue-based waypoint management system.

#include "WaypointQueue.hpp"
#include <iostream>
#include <chrono>

// Adds a waypoint to the queue
void WaypointQueue::add_waypoint(Datatypes::Coordinate waypoint) {
    {
        lock_guard<mutex> lock(queue_mutex);
        waypoints.push(waypoint);
    }
    queue_condition.notify_one(); // Notify the thread that a new waypoint is available
}

// Retrieves and removes the next waypoint from the queue
Datatypes::Coordinate WaypointQueue::get_next_waypoint() {
    lock_guard<mutex> lock(queue_mutex);
    if (!waypoints.empty()) {
        Datatypes::Coordinate next = waypoints.front(); // Get the next waypoint in the queue
        waypoints.pop(); // Remove the waypoint from the queue
        return next; // Return the retrieved waypoint
    } else {
        return {0.0, 0.0}; // Placeholder for empty queue
    }
}

// Checks if the queue is empty
bool WaypointQueue::is_empty() const {
    lock_guard<mutex> lock(queue_mutex);
    return waypoints.empty();
}

// Clears all waypoints from the queue (Emergency override)
void WaypointQueue::clear_queue() {
    lock_guard<mutex> lock(queue_mutex);
    while (!waypoints.empty()) {
        waypoints.pop(); // Remove each waypoint from the queue
    }
}

// Initializes the autonomous mode and starts the execution thread
void WaypointQueue::initialize_autonomous_mode() {
    autonomous_mode = true;
    thread execution_thread(&WaypointQueue::execute_waypoints, this);
    execution_thread.detach(); // Detach the thread to let it run independently
}

// Function to execute waypoints in the queue
void WaypointQueue::execute_waypoints() {
    while (autonomous_mode) {
        unique_lock<mutex> lock(queue_mutex);
        queue_condition.wait(lock, [this]() { return !waypoints.empty() || !autonomous_mode; });

        if (!waypoints.empty()) {
            // Retrieve the next waypoint
            Datatypes::Coordinate next_waypoint = waypoints.front();
            waypoints.pop();
            lock.unlock();

        }
    }
}
