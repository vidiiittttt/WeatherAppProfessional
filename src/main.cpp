#include <iostream>
#include "config/Config.hpp"

int main()
{
    Config config("config/config.json");

    if (!config.load())
    {
        std::cerr << "Failed to load configuration.\n";
        return 1;
    }

std::cout << "Configuration loaded successfully!\n\n";

std::cout << "Default City: " << config.getDefaultCity() << "\n";
std::cout << "Units: " << config.getUnits() << "\n";
std::cout << "Language: " << config.getLanguage() << "\n";

    return 0;
}
