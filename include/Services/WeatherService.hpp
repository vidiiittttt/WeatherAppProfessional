#pragma once 

#include <string>
#include <vector>

#include "config/Config.hpp"
#include "models/WeatherData.hpp"
#include "network/HttpClient.hpp"
#include "models/ForecastData.hpp"

class WeatherService
{
    public:
    explicit WeatherService(const Config&config);

    WeatherData getCurrentWeather(const std::string& city);
    std::vector<ForecastData> getForecast(const std::string& city);

    private:
    Config m_config;
    HttpClient m_httpClient;
};