#include "StompProtocol.h"

StompProtocol::StompProtocol() : connectionHandler_("", 0), host_(""), port_(0), connected(false) {}

StompProtocol::~StompProtocol()
{
    if (connected)
    {
        connectionHandler_.close();
    }
}

bool StompProtocol::connect(string &host, short port, string &username, string &password)
{
    connectionHandler_.setHost(host);
    connectionHandler_.setPort(port);

    if (!connectionHandler_.connect())
    {
        connectionHandler_.setHost("");
        connectionHandler_.setPort(0);
        return false;
    }

    host_ = host;
    port_ = port;
    connected = true;
    return true;
}

void StompProtocol::disconnect()
{
    if (connected)
    {
        connectionHandler_.close();
        connectionHandler_.setHost("");
        connectionHandler_.setPort(0);
        connected = false;
    }
}

void StompProtocol::sendFrame(const string &frame)
{
    connectionHandler_.sendFrameAscii(frame, '\0');
}

Frame StompProtocol::receiveFrame()
{
    string frameString;
    connectionHandler_.getFrameAscii(frameString, '\0');
    return Frame::parseFrame(frameString);
}

string StompProtocol::getHost() const
{
    return host_;
}

short StompProtocol::getPort() const
{
    return port_;
}

bool StompProtocol::isConnected() const
{
    return connected;
}
