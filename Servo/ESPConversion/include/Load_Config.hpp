#ifndef LOAD_CONFIG_HPP
#define LOAD_CONFIG_HPP

#include <string>
#include <nlohmann/json.hpp>

class ConfigLoader 
{
public:
    static nlohmann::json loadConfig();
};

#endif // LOAD_CONFIG_HPP