#pragma once

#include <string>
#include <vector>

class ClientUtils
{
public:
    static void split_str(const std::string &s, char delimiter, std::vector<std::string> &tokens);
    static std::string epochToDateTimeString(int epoch);
};
