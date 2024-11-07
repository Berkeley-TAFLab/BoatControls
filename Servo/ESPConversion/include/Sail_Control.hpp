#ifndef SAIL_CONTROL_HPP
#define SAIL_CONTROL_HPP

#include <vector>
#include <string>

struct SailData {
    int windAngle;
    int optimalSailPosition;
};

// Loads sail data from a CSV file into a vector of SailData structs
std::vector<SailData> loadSailData(const std::string& filename);

// Interpolates the sail position between two SailData points for a given wind angle
int interpolateSailPosition(int windAngle, const SailData& lower, const SailData& upper);

// Returns the optimal sail position for a given wind angle, using interpolation if necessary
int getOptimalSailPosition(int windAngle, const std::vector<SailData>& sailData);

void sail_control_task(void* parameter);

std::vector<SailData> sailData;

#endif // SAIL_CONTROL
