#include "Frame.h"

Frame::Frame(const string &command) : command(command) {}

void Frame::addHeader(const string &key, const string &value)
{
    headers[key] = value;
}

void Frame::setBody(const string &body)
{
    this->body = body;
}

string Frame::getCommand() const
{
    return command;
}

string Frame::getHeader(const string &key) const
{
    auto it = headers.find(key);
    return (it != headers.end()) ? it->second : "";
}

string Frame::getBody() const
{
    return body;
}

string Frame::toString() const
{
    string result = command + "\n";
    for (const auto &header : headers)
    {
        result += header.first + ": " + header.second + "\n";
    }
    result += "\n" + body + "\n^@\n";
    return result;
}