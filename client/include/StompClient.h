#pragma once
#include "StompProtocol.h"
#include "event.h"
#include "Frame.h"

#include <unordered_map>
#include <string>
#include <condition_variable>
#include <mutex>
using namespace std;

class StompClient
{
private:
    StompProtocol stompProtocol_;

    bool loggedIn_;
    string username_;
    string password_;

    int nextSubscriptionID_;
    int nextReceiptID_;
    unordered_map<string, int> channelToSubscriptionID_;
    // todo: thread protect this
    unordered_map<string, vector<Event>> channelToEvents_;

    mutable mutex responseLock_;        // Synchronization lock
    condition_variable_any responseCV_; // Condition variable
    Frame lastResponse_;                // Stores last non-MESSAGE frame
    bool lastResponseUpdated_;          // Flag to indicate if lastResponse_ is updated

public:
    /**
     * @brief Constructs a new StompClient object.
     */
    StompClient();

    /**
     * @brief Destroys the StompClient object.
     */
    ~StompClient();

    /**
     * @brief Connects to the STOMP server.
     *
     * @param host The hostname of the STOMP server.
     * @param port The port number of the STOMP server.
     * @param username The username for authentication.
     * @param password The password for authentication.
     */
    void login(string &host, short port, string &username, string &password);

    /**
     * @brief Disconnects from the STOMP server.
     */
    void logout();

    /**
     * @brief Subscribes to a specified destination.
     *
     * @param destination The destination to subscribe to.
     */
    void join(string &destination);

    /**
     * @brief Unsubscribes from a specified destination.
     *
     * @param destination The destination to unsubscribe from.
     */
    void exit(string &destination);

    /**
     * @brief Reports events from a specified file.
     *
     * @param filePath The path to the file containing events.
     */
    void report(string &filePath);

    /**
     * @brief Sends a message to a specified destination.
     *
     * @param destination The destination to send the message to.
     * @param body The body of the message.
     */
    void send(string &destination, string &body);

    /**
     * @brief Summarizes events for a specified channel and user, and writes the summary to an output file.
     *
     * @param channelName The name of the channel.
     * @param user The user for whom the summary is generated.
     * @param outputFile The file to write the summary to.
     */
    void summarize(string &channelName, string &user, string &outputFile);

    /**
     * @brief Listens for incoming messages from the server.
     *
     * This function continuously listens for messages from the server and processes them accordingly.
     * It should be run in a separate thread to avoid blocking the main execution flow.
     */
    void listen();

    /**
     * @brief Gets the last response(non-MESSAGE) frame received from the server.
     *
     * @return Frame The last response frame.
     */
    Frame getResponse();

    /**
     * @brief Gets the STOMP protocol.
     *
     * @return StompProtocol* The STOMP protocol.
     */
    StompProtocol *getProtocol() const;

    /**
     * @brief Checks if the client is logged in.
     *
     * @return bool True if logged in, false otherwise.
     */
    bool isLoggedIn() const;

    /**
     * @brief Gets the username.
     *
     * @return string The username.
     */
    string getUsername() const;

    /**
     * @brief Gets the password.
     *
     * @return string The password.
     */
    string getPassword() const;

    /**
     * @brief Gets the next subscription ID.
     *
     * @return int The next subscription ID.
     */
    int getNextSubscriptionID() const;

    /**
     * @brief Generates the next subscription ID.
     *
     * This function generates a unique subscription ID for the next subscription.
     *
     * @return A string representing the next subscription ID.
     */
    string generateNextSubscriptionID();

    /**
     * @brief Gets the next receipt ID.
     *
     * @return int The next receipt ID.
     */
    int getNextReceiptID() const;

    /**
     * @brief Generates the next unique receipt ID.
     *
     * This function is responsible for creating a new, unique receipt ID
     * each time it is called. The receipt ID is typically used to track
     * and acknowledge messages in the STOMP protocol.
     *
     * @return A string representing the next unique receipt ID.
     */
    string generateNextReceiptID();

    /**
     * @brief Gets the channel to subscription ID map.
     *
     * @return unordered_map<string, int> The channel to subscription ID map.
     */
    unordered_map<string, int> getChannelToSubscriptionID() const;

    /**
     * @brief Gets the channel to events map.
     *
     * @return unordered_map<string, vector<Event>> The channel to events map.
     */
    unordered_map<string, vector<Event>> getChannelToEvents() const;
};
