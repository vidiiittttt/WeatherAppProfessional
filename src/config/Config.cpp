
#include "config/Config.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

Config::Config(const std::string& filePath)
    : m_filePath(filePath)
{
}

bool Config::load()
{
    std::ifstream file(m_filePath);

    if (!file.is_open())
    {
        return false;
    }

    try
    {
        json config = json::parse(file);

        if (!config.is_object())
        {
            return false;
        }

        m_openWeatherApiKey =
            config.value("openWeatherApiKey", "");

        m_weatherApiKey =
            config.value("weatherApiKey", "");

        m_units =
            config.value("units", "metric");

        m_defaultCity =
            config.value("defaultCity", "Delhi");

        m_language =
            config.value("language", "en");

        if (m_units != "metric" &&
            m_units != "imperial" &&
            m_units != "standard")
        {
            return false;
        }

        if (m_defaultCity.empty() ||
            m_language.empty())
        {
            return false;
        }

        return true;
    }
    catch (const json::exception&)
    {
        return false;
    }
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

void Config::setUnits(const std::string& units)
{
    m_units = units;
}

std::string Config::getDefaultCity() const
{
    return m_defaultCity;
}

std::string Config::getLanguage() const
{
    return m_language;
}
