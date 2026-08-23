#include <iostream>
#include "Services/WeatherService.hpp"
#include <stdexcept>
#include <sstream>
#include <nlohmann/json.hpp>
#include <algorithm>

using json = nlohmann::json;

WeatherService::WeatherService(const Config& config)
    : m_config(config)
{
}

WeatherData WeatherService::getCurrentWeather(const std::string& city)
{
    std::stringstream url;
    
    url << "https://api.openweathermap.org/data/2.5/weather?q=" << city
        << "&appid=" << m_config.getOpenWeatherApiKey()
        << "&units=" << m_config.getUnits()
        << "&lang=" << m_config.getLanguage();

        std::string response = m_httpClient.get(url.str());
        
        json weatherJson = json::parse(response);
        if (weatherJson.contains("cod"))
        {
            int code;

            if (weatherJson["cod"].is_number())
                {
                code = weatherJson["cod"].get<int>();
                }
                else if (weatherJson["cod"].is_string())
                {
                code = std::stoi(weatherJson["cod"].get<std::string>());
                }
                else
                {
                throw std::runtime_error("Invalid API response.");
            }
            if (code != 200)
            {
                std::string message = weatherJson.value("message", "Unknown error");
                throw std::runtime_error("Weather request failed: " + message);
            }
        }

        WeatherData weather;
            

        weather.city = weatherJson["name"];
        weather.description = weatherJson["weather"][0]["description"];

        weather.temperature = weatherJson["main"]["temp"];
        weather.feels_like = weatherJson["main"]["feels_like"];
        weather.temp_min = weatherJson["main"]["temp_min"];
        weather.temp_max = weatherJson["main"]["temp_max"];

        weather.humidity = weatherJson["main"]["humidity"];
        weather.pressure = weatherJson["main"]["pressure"];
        weather.wind_speed = weatherJson["wind"]["speed"];

       

        return weather;
}

std::vector<ForecastData> WeatherService::getForecast(const std::string& city)
{
    std::stringstream url;

    url << "http://api.openweathermap.org/data/2.5/forecast?q=" <<city
    << "&appid=" <<m_config.getOpenWeatherApiKey()
    << "&units=" <<m_config.getUnits()
    << "&lang=" << m_config.getLanguage();

    std::string response = m_httpClient.get(url.str());

    json forecastJson = json::parse(response);

    if (forecastJson.contains("cod"))
    {
        int code;

        if (forecastJson["cod"].is_number())
        {
            code = forecastJson["cod"].get<int>();
        }
        else if (forecastJson["cod"].is_string())
        {
            code = std::stoi(forecastJson["cod"].get<std::string>());
        }
        else{
            throw std::runtime_error("Invalid API response.");
        }

        if (code != 200)
        {
            std::string message =
            forecastJson.value("message", "Unkown error");

            throw std::runtime_error("Forecast request failed: "
            + message);
        }
    }

    std::vector<ForecastData> forecasts;

    for (const auto& item : forecastJson["list"])
    {
        std::string date = item["dt_txt"];
        date = date.substr(0, 10);

        double temp = item["main"]["temp"];

        auto it = std::find_if(
            forecasts.begin(),
            forecasts.end(),
            [&](const ForecastData& f)
            {
                return f.date == date;
            }
        );

        if (it == forecasts.end())
        {
            ForecastData forecast;

            forecast.date = date;
            forecast.description = item["weather"][0]["description"];

            forecast.temperature = temp;
            forecast.feels_like = item["main"]["feels_like"];

            //Start the day's min/max with the forecast entry
            forecast.temp_min = item["main"]["temp_min"];
            forecast.temp_max = item["main"]["temp_max"];

            forecast.humidity = item["main"]["humidity"];
            forecast.pressure = item["main"]["pressure"];
            forecast.wind_speed = item["wind"]["speed"];

            forecasts.push_back(forecast);
        }
        else
        {
            // Update daily min/max using every 3-hour forecast
            it->temp_min = std::min(
                it->temp_min,
                static_cast<double>(item["main"]["temp_min"])
            );

            it->temp_max = std::max(
                it->temp_max,
                static_cast<double>(item["main"]["temp_max"])
            );
        }
    }
  return forecasts;
}