#pragma once

#include <string>

struct WeatherData 
{
    std::string city;
    std::string description;

    double temperature;
    double feels_like;
    double temp_min;
    double temp_max;

int humidity;
    int pressure;
    double wind_speed;
};