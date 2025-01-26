#include "ClientUtils.h"
#include <sstream>
#include <ctime>
#include <iomanip>

void ClientUtils::split_str(const std::string &s, char delimiter, std::vector<std::string> &tokens)
{
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
}

std::string ClientUtils::epochToDateTimeString(int epoch)
{
    std::time_t time = static_cast<std::time_t>(epoch);
    std::tm *timeinfo = std::localtime(&time);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << timeinfo->tm_mday << "/"
        << std::setw(2) << (timeinfo->tm_mon + 1) << "/"
        << (timeinfo->tm_year + 1900) << " "
        << std::setw(2) << timeinfo->tm_hour << ":"
        << std::setw(2) << timeinfo->tm_min;

    return oss.str();
}
