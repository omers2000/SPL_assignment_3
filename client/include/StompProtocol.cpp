#include "StompProtocol.h"

StompProtocol::StompProtocol() {}

StompProtocol::~StompProtocol()
{
}

bool StompProtocol::connect(string &host, short port, string &username, string &password)
{

    // todo: this may be unnecessary
    if (connectionHandler_ != nullptr)
    {
        delete connectionHandler_;
    }
    connectionHandler_ = new ConnectionHandler(host, port);

    if (!connectionHandler_->connect())
    {
        delete connectionHandler_;
        connectionHandler_ = nullptr;
        return false;
    }

    host_ = host;
    port_ = port;
    return true;
}

void StompProtocol::disconnect()
{
    if (connectionHandler_ != nullptr)
    {
        connectionHandler_->close();
        delete connectionHandler_;
        connectionHandler_ = nullptr;
    }
}

void StompProtocol::sendFrame(const string &frame)
{
    connectionHandler_->sendFrameAscii(frame, '\0');
}

Frame StompProtocol::receiveFrame()
{
    string frameString;
    connectionHandler_->getFrameAscii(frameString, '\0');
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

ConnectionHandler *StompProtocol::getConnectionHandler() const
{
    return connectionHandler_;
}
