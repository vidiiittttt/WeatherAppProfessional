#pragma once

#include <string>

struct ForecastData 
{
    std::string date;
    std::string description;

    double temperature;
    double feels_like;
    double temp_min;
    double temp_max;

    int humidity;
    int pressure;
    double wind_speed;
};