
#include "network/HttpClient.hpp"
#include "exceptions/HttpException.hpp"

#include <curl/curl.h>
#include <string>

static size_t writeCallback(
    void* contents,
    size_t size,
    size_t nmemb,
    void* userData)
{
    size_t totalSize = size * nmemb;

    std::string* response =
        static_cast<std::string*>(userData);

    response->append(
        static_cast<char*>(contents),
        totalSize
    );

    return totalSize;
}

HttpClient::HttpClient()
{
    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);

    if (result != CURLE_OK)
    {
        throw HttpException(
            "Failed to initialize CURL."
        );
    }
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
        throw HttpException(
            "Failed to initialize CURL request."
        );
    }

    std::string response;

    auto setOption = [&](CURLoption option, auto value)
    {
        CURLcode result = curl_easy_setopt(curl, option, value);

        if (result != CURLE_OK)
        {
            std::string error = curl_easy_strerror(result);
            curl_easy_cleanup(curl);

            throw HttpException(
                "CURL configuration error: " + error
            );
        }
    };

    setOption(CURLOPT_URL, url.c_str());
    setOption(CURLOPT_WRITEFUNCTION, writeCallback);
    setOption(CURLOPT_WRITEDATA, &response);
    setOption(CURLOPT_FOLLOWLOCATION, 1L);

    // Connection timeout: 10 seconds
    setOption(CURLOPT_CONNECTTIMEOUT, 10L);

    // Total request timeout: 30 seconds
    setOption(CURLOPT_TIMEOUT, 30L);

    // Verify SSL certificates
    setOption(CURLOPT_SSL_VERIFYPEER, 1L);
    setOption(CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode result = curl_easy_perform(curl);

    if (result != CURLE_OK)
    {
        std::string error = curl_easy_strerror(result);

        curl_easy_cleanup(curl);

        throw HttpException(
            "Network error: " + error
        );
    }

    long httpStatusCode = 0;

    result = curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &httpStatusCode
    );

    if (result != CURLE_OK)
    {
        std::string error = curl_easy_strerror(result);

        curl_easy_cleanup(curl);

        throw HttpException(
            "Failed to retrieve HTTP status: " + error
        );
    }

    curl_easy_cleanup(curl);

    return response;
}
