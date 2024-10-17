// Filename: WaypointQueue.cpp
// Author: Kieran Pereira
// Date: 10/11/2024
// Description: Implementation file for the WaypointQueue class, providing a queue-based waypoint management system.

#include <iostream>
#include <chrono>
#include <cmath>

#include "WaypointQueue.hpp"
#include "Coordinate_Calculations.h"
#include "TAF_AS5600.h"
#include "TAF_GTU7.h"
#include "Boat_steer.h"

void WaypointQueue::initialize_autonomous_mode()
{
    lock_guard<mutex> lock(queue_mutex);
    autonomous_mode = true;
    execution_thread = thread(&WaypointQueue::execute_waypoints, this);
}

Datatypes::Coordinate WaypointQueue::get_next_waypoint()
{
    unique_lock<mutex> lock(queue_mutex);
    while (waypoints.empty() && autonomous_mode)
    {
        queue_condition.wait(lock);
    }
    if (!waypoints.empty())
    {
        Datatypes::Coordinate next_waypoint = waypoints.front();
        waypoints.pop();
        return next_waypoint;
    }
    return Datatypes::Coordinate(); // Return default coordinate if queue is empty
}

void WaypointQueue::add_waypoint(Datatypes::Coordinate waypoint)
{
    lock_guard<mutex> lock(queue_mutex);
    waypoints.push(waypoint);
    queue_condition.notify_one();
}

void WaypointQueue::clear_queue()
{
    lock_guard<mutex> lock(queue_mutex);
    while (!waypoints.empty())
    {
        waypoints.pop();
    }
}

bool WaypointQueue::is_empty() const
{
    lock_guard<mutex> lock(queue_mutex);
    return waypoints.empty();
}

void WaypointQueue::execute_waypoints()
{
    while (autonomous_mode)
    {
        // Get the next waypoint from the queue
        Datatypes::Coordinate waypoint = get_next_waypoint();

        bool reached = false;

        while (!reached && autonomous_mode)
        {
            // Update current position from GPS
            Datatypes::Coordinate curr_position = get_curr_coordinate();
            
            // Calculate distance to the waypoint
            CoordinateCalcuations execute_coordinatecalcs;
            float distance = execute_coordinatecalcs.calculate_distance(curr_position, waypoint);
            
            // If we have reached the waypoint (distance below a threshold), set 'reached' to true
            if (distance <= 0.005) // 0.005 can be adjusted based on acceptable proximity
            {
                reached = true;
                continue;  
            }

            // Calculate bearing from current position to waypoint
            float bearing = execute_coordinatecalcs.calculate_bearing(curr_position, waypoint);

            // Steer boat towards the waypoint
            steering_boat(bearing);

            // Wait for a bit before updating again to prevent excessive looping
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // If no waypoints and close to previous destination, point sail into wind
        if (reached && waypoints.empty())
        {
            CoordinateCalcuations coordcalc;
            float distance = coordcalc.calculate_distance(waypoint, get_curr_coordinate());
            if (abs(distance) <= 0.005)
            {
                set_rudder_servo(get_avg_angle());
            }
        }
    }
}

void WaypointQueue::close_autonomous_mode()
{
    {
        lock_guard<mutex> lock(queue_mutex);
        autonomous_mode = false;
        queue_condition.notify_all();
    }
    if (execution_thread.joinable())
    {
        execution_thread.join();
    }
    clear_queue();
}
