#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include "models/ForecastData.hpp"
#include "config/Config.hpp"
#include "Services/WeatherService.hpp"

int main()
{
    try
    {
        Config config("config/config.json");
    
    if (!config.load())
{
    std::cerr << "Failed to load configuration.\n";
    return 1;
}
if (config.getOpenWeatherApiKey().empty())
{
    std::cerr << "Error: OpenWeather API key is missing. "
              << "Please configure it in config/config.json.\n";
    return 1;
}
    
    std::string unitChoice;
    std::cout << "Choose temperature unit (C for Celsius, F for Fahrenheit): ";
    std::getline(std::cin, unitChoice);

    if (unitChoice == "C" || unitChoice == "c")
    {
        config.setUnits("metric");
    }
    else if (unitChoice == "F" || unitChoice == "f")
    {
        config.setUnits("imperial");
    }
    else
    {
        std::cerr << "Invalid choice. Defaulting to Celsius.\n";
        config.setUnits("metric");
    }
    
    WeatherService weatherService(config);
    std::string city;
        std::cout << "Enter city:";
        std::getline(std::cin, city);

        city.erase(city.begin(), std::find_if(city.begin(), city.end(), 
    [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    
        city.erase(std::find_if(city.rbegin(), city.rend(), 
    [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), 
    city.end());

        if (city.empty())
        {
            std::cerr << "City name cannot be empty.\n";
            return 1;
        }
        WeatherData weather =
        weatherService.getCurrentWeather(city);
        

        std::cout << "\nCurrent Weather\n";
        std::string unitSymbol = (config.getUnits() == "imperial") ? "°F" : "°C";
        std::cout << "-----------------\n";
        std::cout << "City: " << weather.city << "\n";
       std::cout << "Temperature: " << weather.temperature << unitSymbol << "\n";
       std::cout << "Feels Like: " << weather.feels_like << unitSymbol << "\n";
       std::cout << "Min Temperature: " << weather.temp_min << unitSymbol << "\n";
       std::cout << "Max Temperature: " << weather.temp_max << unitSymbol << "\n";
        std::cout << "Humidity: " << weather.humidity << "%\n";
        std::cout << "Pressure: " << weather.pressure << " hPa\n";
       std::string windUnit = (config.getUnits() == "imperial") ? "mph" : "m/s";
        std::cout << "Wind Speed: " << weather.wind_speed << " " << windUnit << "\n";
        std::cout << "Condition: " << weather.description << "\n";
        std::vector<ForecastData> forecasts =
    weatherService.getForecast(city);

std::cout << "\nForecast\n";
std::cout << "-----------------\n";

for (const auto& forecast : forecasts)
{
    std::cout << forecast.date << " | "
             << "Min: " << forecast.temp_min << unitSymbol
             << " | Max: " << forecast.temp_max << unitSymbol
             << " | " << forecast.description << "\n";
}
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}