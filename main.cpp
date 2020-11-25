#include <iostream>
#include <string>

#include <curl/curl.h>
#include <nlohmann/json.hpp>

size_t responseHandler(void* deliveredData,
                       size_t size,
                       size_t deliveredDataSize,
                       void* responseBuffer)
{
    const auto deliveredDataActualSize = size * deliveredDataSize;

    *static_cast<std::string*>(responseBuffer) = std::string(
        static_cast<const char*>(deliveredData), deliveredDataActualSize);

    return deliveredDataActualSize;
}

int main()
{
    CURL* curl = curl_easy_init();

    curl_easy_setopt(
        curl, CURLOPT_URL, "http://dummy.restapiexample.com/api/v1/employee/1");

    std::string response{};
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, responseHandler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    const auto curlSuccess = curl_easy_perform(curl) == CURLE_OK;
    curl_easy_cleanup(curl);

    if (!curlSuccess)
    {
        std::cerr << "Could not reach website to fetch employee data"
                  << std::endl;
        return 1;
    }

    auto employeeData = nlohmann::json::parse(response);
    if (employeeData["status"] != "success")
    {
        std::cerr << "Error while fetching employee data" << std::endl;
        return 1;
    }

    std::cout << "ID: " << employeeData["data"]["id"] << std::endl;
    std::cout << "Name: " << employeeData["data"]["employee_name"] << std::endl;
    std::cout << "Age: " << employeeData["data"]["employee_age"] << std::endl;

    return 0;
}
