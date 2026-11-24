#pragma once

#include <stdexcept>
#include <string>

class HttpException : public std::runtime_error
{
public:
    explicit HttpException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};
