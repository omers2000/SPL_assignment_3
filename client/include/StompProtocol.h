#pragma once

#include "../include/ConnectionHandler.h"
#include <string>
#include "Frame.h"
using namespace std;

class StompProtocol
{
private:
    ConnectionHandler *connectionHandler_;
    string host_;
    short port_;

public:
    /**
     * @brief Constructs a new StompProtocol object.
     */
    StompProtocol();

    /**
     * @brief Destroys the StompProtocol object.
     */
    ~StompProtocol();

    /**
     * @brief Connects to the STOMP server.
     *
     * @param host The hostname of the STOMP server.
     * @param port The port number of the STOMP server.
     * @param username The username for authentication.
     * @param password The password for authentication.
     */
    bool connect(string &host, short port, string &username, string &password);

    /**
     * @brief Disconnects from the STOMP server.
     */
    void disconnect();

    /**
     * @brief Sends a STOMP frame to the server.
     *
     * This function takes a STOMP frame as a string and sends it to the server.
     *
     * @param frame The STOMP frame to be sent. It should be a properly formatted
     *              STOMP frame string.
     */
    void sendFrame(const string &frame);

    /**
     * @brief Receives a frame from the STOMP server.
     *
     * This function waits for and retrieves the next available frame from the
     * STOMP server. It blocks until a frame is received.
     *
     * @return Frame The received frame.
     */
    Frame receiveFrame();

    /**
     * @brief Gets the connection handler.
     *
     * @return ConnectionHandler* The connection handler.
     */
    ConnectionHandler *getConnectionHandler() const;

    /**
     * @brief Gets the host.
     *
     * @return string The host.
     */
    string getHost() const;

    /**
     * @brief Gets the port.
     *
     * @return short The port.
     */
    short getPort() const;
};
