#pragma once

#include "../include/ConnectionHandler.h"
#include <string>
#include "Frame.h"
using namespace std;

// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    ConnectionHandler *connectionHandler_;
    string host_;
    short port_;

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
    void connect(string &host, short port, string &username, string &password);

    /**
     * @brief Sends a message to a specified destination.
     *
     * @param destination The destination to send the message to.
     * @param body The body of the message.
     */
    void send(string &destination, string &body);

    /**
     * @brief Subscribes to a specified destination.
     *
     * @param destination The destination to subscribe to.
     */
    void subscribe(string &destination);

    /**
     * @brief Unsubscribes from a specified destination.
     *
     * @param destination The destination to unsubscribe from.
     */
    void unsubscribe(string &destination);

    /**
     * @brief Disconnects from the STOMP server.
     */
    void disconnect();

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
