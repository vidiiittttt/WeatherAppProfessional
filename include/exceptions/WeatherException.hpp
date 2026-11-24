#pragma once

#include <stdexcept>
#include <string>

class WeatherException : public std::runtime_error
{
public:
    explicit WeatherException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};
