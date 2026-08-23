#include "network/HttpClient.hpp"

#include <curl/curl.h>
#include <stdexcept>

static size_t writeCallback(void* contents,
size_t size,
size_t nmemb,
void* userData)
{
    size_t totalSize = size * nmemb;

    std::string* response = static_cast<std::string*>(userData);
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

HttpClient::HttpClient()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HttpClient::~HttpClient()
{
    curl_global_cleanup();
}

std::string HttpClient::get(const std::string& url) 
{
    
    CURL* curl = curl_easy_init();
    
    if (!curl)
    {
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::string error = curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL error: " + error);
    }

    curl_easy_cleanup(curl);

    return response;
}