#include "ConnectionHandler.h"
#include "event.h"
#include "Frame.h"

#include <unordered_map>
#include <string>
using namespace std;

class StompClient
{
private:
    ConnectionHandler *connectionHandler_;
    string host_;
    short port_;
    bool loggedIn_;

    string username_;
    string password_;
    int nextSubscriptionID_;
    int nextReceiptID_;
    unordered_map<string, int> channelToSubscriptionID_;
    unordered_map<string, vector<Event>> channelToEvents_;

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
    void connect(string &host, short port, string &username, string &password);

    /**
     * @brief Sends a message to a specified destination.
     *
     * @param destination The destination to send the message to.
     * @param body The body of the message.
     */
    void send(string &destination, string &body);

    /**
     * @brief Receives events from the server.
     *
     * This function continuously listens for incoming frames from the server
     * and processes them as events.
     */
    void receive();

    /**
     * @brief Reports events from a specified file.
     *
     * @param filePath The path to the file containing events.
     */
    void report(string &filePath);

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
     * @brief Summarizes events for a specified channel and user, and writes the summary to an output file.
     *
     * @param channelName The name of the channel.
     * @param user The user for whom the summary is generated.
     * @param outputFile The file to write the summary to.
     */
    void summarize(string &channelName, string &user, string &outputFile);

    /**
     * @brief Gets the connection handler.
     * 
     * @return ConnectionHandler* The connection handler.
     */
    ConnectionHandler* getConnectionHandler() const;

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
     * @brief Gets the next receipt ID.
     * 
     * @return int The next receipt ID.
     */
    int getNextReceiptID() const;

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

