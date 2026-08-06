#include "config/Config.hpp"

#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

Config::Config(const std::string& filePath) : m_filePath(filePath) {}

bool Config::load() 
{
    std::ifstream file(m_filePath);

    if (!file.is_open())
    {
        return false;

    }

    json config;
    file >> config;

    m_openWeatherApiKey = config["openWeatherApiKey"];
    m_weatherApiKey = config["weatherApiKey"];
    m_units = config["units"];
    m_defaultCity = config["defaultCity"];
    m_language = config["language"];

    return true;
}

std::string Config::getOpenWeatherApiKey() const 
{
    return m_openWeatherApiKey;
}

std::string Config::getWeatherApiKey() const 
{
    return m_weatherApiKey;
}

std::string Config::getUnits() const 
{
    return m_units;
}

std::string Config::getDefaultCity() const 
{
    return m_defaultCity;
}

std::string Config::getLanguage() const 
{
    return m_language;
}