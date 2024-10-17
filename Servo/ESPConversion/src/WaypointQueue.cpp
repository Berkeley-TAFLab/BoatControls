// Filename: WaypointQueue.cpp
// Author: Kieran Pereira
// Date: 10/11/2024
// Description: Implementation file for the WaypointQueue class, providing a queue-based waypoint management system.

#include <iostream>
#include <chrono>
#include <cmath>
#include <HardwareSerial.h>

#include "WaypointQueue.hpp"
#include "Coordinate_Calculations.h"
#include "TAF_AS5600.h"
#include "TAF_GTU7.h"
#include "Boat_steer.h"


void WaypointQueue::initialize_autonomous_mode()
{
    lock_guard<mutex> lock(queue_mutex);
    autonomous_mode = true;
    running = true;
    execution_thread = thread(&WaypointQueue::execute_waypoints, this);  // Start the execution thread
}

Datatypes::Coordinate WaypointQueue::get_next_waypoint()
{
    unique_lock<mutex> lock(queue_mutex);
    while (waypoints.empty() && running) {
        
        // If at previous destination, point sail into wind
        Datatypes::Coordinate current_position = get_curr_coordinate();
        CoordinateCalcuations coordcalc;
        float distance = coordcalc.calculate_bearing(previous_waypoint, current_position);
        if (abs(distance) <= 0.005)
        {
            set_sail_servo(get_avg_angle());
            set_rudder_servo(get_avg_angle());
        }else
        {
            Serial.println("WaypointQueue.cpp ln 40: Need to move");
        }

        queue_condition.wait(lock);
    }
    if (!waypoints.empty()) {
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
    while (running)
    {
        Datatypes::Coordinate waypoint = get_next_waypoint();
        Datatypes::Coordinate curr_position = _get_current_locale();
        // Execute movement to the waypoint
        float bearing = CoordinateCalcuations::calculate_bearing(curr_position,waypoint);
        

    }
}

void WaypointQueue::close_autonomous_mode()
{
    {
        lock_guard<mutex> lock(queue_mutex);
        autonomous_mode = false;
        running = false;
        queue_condition.notify_all();
    }
    if (execution_thread.joinable())
    {
        execution_thread.join();
    }
    clear_queue();
}
