#include "Frame.h"

Frame::Frame(const string &command) : command(command), headers(), body() {}

Frame::~Frame() = default;

Frame::Frame(const Frame &other) : command(other.command), headers(other.headers), body(other.body) {}

Frame::Frame(Frame &&other) noexcept : command(std::move(other.command)), headers(std::move(other.headers)), body(std::move(other.body)) {}

Frame &Frame::operator=(const Frame &other)
{
    if (this != &other)
    {
        command = other.command;
        headers = other.headers;
        body = other.body;
    }
    return *this;
}

Frame &Frame::operator=(Frame &&other) noexcept
{
    if (this != &other)
    {
        command = std::move(other.command);
        headers = std::move(other.headers);
        body = std::move(other.body);
    }
    return *this;
}
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

Frame Frame::parseFrame(const string &frameString)
{
    vector<string> lines = splitIntoLines(frameString);

    Frame frame(lines[0]);
    for (int i = 1; i < lines.size() - 1; i++)
    {
        int colonPos = lines[i].find(':');
        string key = lines[i].substr(0, colonPos);
        string value = lines[i].substr(colonPos + 1);
        frame.addHeader(key, value);
    }

    frame.setBody(lines[lines.size() - 1]);
    return frame;
}

vector<string> Frame::splitIntoLines(const string &input)
{
    vector<string> lines;
    istringstream stream(input);
    string line;

    while (getline(stream, line))
    {
        lines.push_back(line);
    }

    return lines;
}

string Frame::toString() const
{
    string result = command + "\n";
    for (const auto &header : headers)
    {
        result += header.first + ": " + header.second + "\n";
    }
    if (!body.empty())
    {
        result += "\n" + body + "\n";
    }
    result += '\0';
    return result;
}