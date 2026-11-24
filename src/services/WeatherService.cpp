
#include "Services/WeatherService.hpp"
#include "exceptions/WeatherException.hpp"
#include "exceptions/HttpException.hpp"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <sstream>
#include <string>

using json = nlohmann::json;

// Encode city names for safe URL requests
static std::string urlEncode(const std::string& value)
{
    CURL* curl = curl_easy_init();

    if (!curl)
    {
        throw HttpException("Failed to initialize URL encoder.");
    }

    char* encoded = curl_easy_escape(
        curl,
        value.c_str(),
        static_cast<int>(value.length())
    );

    if (!encoded)
    {
        curl_easy_cleanup(curl);
        throw HttpException("Failed to encode city name.");
    }

    std::string result(encoded);

    curl_free(encoded);
    curl_easy_cleanup(curl);

    return result;
}

// Parse and validate API response
static json parseResponse(
    const std::string& response,
    const std::string& requestType)
{
    json data;

    try
    {
        data = json::parse(response);
    }
    catch (const json::parse_error&)
    {
        throw WeatherException(
            "Weather service returned invalid JSON."
        );
    }

    if (!data.is_object())
    {
        throw WeatherException("Invalid API response.");
    }

    if (data.contains("cod"))
    {
        int code = -1;

        if (data["cod"].is_number_integer())
        {
            code = data["cod"].get<int>();
        }
        else if (data["cod"].is_string())
        {
            try
            {
                std::string codeText =
                    data["cod"].get<std::string>();

                size_t position = 0;
                code = std::stoi(codeText, &position);

                if (position != codeText.size())
                {
                    throw WeatherException(
                        "Invalid API response status."
                    );
                }
            }
            catch (const std::invalid_argument&)
            {
                throw WeatherException(
                    "Invalid API response status."
                );
            }
            catch (const std::out_of_range&)
            {
                throw WeatherException(
                    "Invalid API response status."
                );
            }
        }
        else
        {
            throw WeatherException("Invalid API response.");
        }

        if (code != 200)
        {
            std::string message = "Unknown error";

            if (data.contains("message") &&
                data["message"].is_string())
            {
                message = data["message"].get<std::string>();
            }

            throw WeatherException(
                requestType + " request failed: " + message
            );
        }
    }

    return data;
}

WeatherService::WeatherService(const Config& config)
    : m_config(config)
{
}

// Fetch current weather
WeatherData WeatherService::getCurrentWeather(
    const std::string& city)
{
    std::stringstream url;

    url << "https://api.openweathermap.org/data/2.5/weather?q="
        << urlEncode(city)
        << "&appid=" << m_config.getOpenWeatherApiKey()
        << "&units=" << m_config.getUnits()
        << "&lang=" << m_config.getLanguage();

    std::string response = m_httpClient.get(url.str());

    json weatherJson = parseResponse(
        response,
        "Weather"
    );

    try
    {
        WeatherData weather;

        weather.city = weatherJson.at("name").get<std::string>();

        weather.description =
            weatherJson.at("weather").at(0)
            .at("description").get<std::string>();

        weather.temperature =
            weatherJson.at("main").at("temp").get<double>();

        weather.feels_like =
            weatherJson.at("main").at("feels_like").get<double>();

        weather.temp_min =
            weatherJson.at("main").at("temp_min").get<double>();

        weather.temp_max =
            weatherJson.at("main").at("temp_max").get<double>();

        weather.humidity =
            weatherJson.at("main").at("humidity").get<int>();

        weather.pressure =
            weatherJson.at("main").at("pressure").get<int>();

        weather.wind_speed =
            weatherJson.at("wind").at("speed").get<double>();

        return weather;
    }
    catch (const json::exception&)
    {
        throw WeatherException(
            "Invalid current weather data received from API."
        );
    }
}

// Fetch weather forecast
std::vector<ForecastData> WeatherService::getForecast(
    const std::string& city)
{
    std::stringstream url;

    url << "https://api.openweathermap.org/data/2.5/forecast?q="
        << urlEncode(city)
        << "&appid=" << m_config.getOpenWeatherApiKey()
        << "&units=" << m_config.getUnits()
        << "&lang=" << m_config.getLanguage();

    std::string response = m_httpClient.get(url.str());

    json forecastJson = parseResponse(
        response,
        "Forecast"
    );

    try
    {
        if (!forecastJson.at("list").is_array())
        {
            throw WeatherException(
                "Invalid forecast data received from API."
            );
        }

        std::vector<ForecastData> forecasts;

        for (const auto& item : forecastJson.at("list"))
        {
            std::string date =
                item.at("dt_txt").get<std::string>();

            if (date.length() < 10)
            {
                throw WeatherException(
                    "Invalid forecast date received from API."
                );
            }

            date = date.substr(0, 10);

            double temp =
                item.at("main").at("temp").get<double>();

            auto it = std::find_if(
                forecasts.begin(),
                forecasts.end(),
                [&](const ForecastData& forecast)
                {
                    return forecast.date == date;
                }
            );

            if (it == forecasts.end())
            {
                ForecastData forecast;

                forecast.date = date;

                forecast.description =
                    item.at("weather").at(0)
                    .at("description").get<std::string>();

                forecast.temperature = temp;

                forecast.feels_like =
                    item.at("main").at("feels_like").get<double>();

                forecast.temp_min =
                    item.at("main").at("temp_min").get<double>();

                forecast.temp_max =
                    item.at("main").at("temp_max").get<double>();

                forecast.humidity =
                    item.at("main").at("humidity").get<int>();

                forecast.pressure =
                    item.at("main").at("pressure").get<int>();

                forecast.wind_speed =
                    item.at("wind").at("speed").get<double>();

                forecasts.push_back(forecast);
            }
            else
            {
                // Update daily minimum and maximum temperatures
                it->temp_min = std::min(
                    it->temp_min,
                    item.at("main").at("temp_min").get<double>()
                );

                it->temp_max = std::max(
                    it->temp_max,
                    item.at("main").at("temp_max").get<double>()
                );
            }
        }

        if (forecasts.empty())
        {
            throw WeatherException(
                "No forecast data available."
            );
        }

        return forecasts;
    }
    catch (const json::exception&)
    {
        throw WeatherException(
            "Invalid forecast data received from API."
        );
    }
}
