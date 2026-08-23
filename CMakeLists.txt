cmake_minimum_required(VERSION 3.20)

project(WeatherAppProfessional VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(WeatherAppProfessional 
src/main.cpp
src/config/Config.cpp
src/network/HttpClient.cpp
src/services/WeatherService.cpp
)

target_include_directories(WeatherAppProfessional
PRIVATE
${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(WeatherAppProfessional
PRIVATE
CURL::libcurl
nlohmann_json::nlohmann_json
)