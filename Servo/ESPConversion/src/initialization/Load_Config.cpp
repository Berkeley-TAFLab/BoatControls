#include <fstream>
#include <iostream>

#include "Load_Config.hpp"


nlohmann::json ConfigLoader::loadConfig() 
{
    std::string config_address = "config.json";
    std::ifstream file(config_address);

    if (!file.is_open()) {
        std::cerr << "Error opening config file: " << config_address << std::endl;
        return nullptr;
    }

    nlohmann::json config;
    file >> config;

    return config;
}
