#pragma once
#include <string>

class Config {

public:
explicit Config(const std::string& filePath);
bool load ();

    std::string getOpenWeatherApiKey() const;
    std::string getWeatherApiKey() const;
    std::string getUnits() const;
    void setUnits(const std::string& units);
    std::string getDefaultCity() const;
    std::string getLanguage() const;

private:
    std::string m_filePath;

    std::string m_openWeatherApiKey;
    std::string m_weatherApiKey;
    std::string m_units;
    std::string m_defaultCity;
    std::string m_language;
};