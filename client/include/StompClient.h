#include "StompProtocol.h"
#include "event.h"
#include "Frame.h"

#include <unordered_map>
#include <string>

using namespace std;

class StompClient
{
private:
    StompProtocol *stompProtocol_;
    bool loggedIn_;

    string username_;
    string password_;
    int nextSubscriptionID_;
    int nextReceiptID_;
    unordered_map<string, int> channelToSubscriptionID_;
    unordered_map<string, vector<Event>> channelToEvents_;

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
     * @brief Disconnects from the STOMP server.
     */
    void logout();

    /**
     * @brief Summarizes events for a specified channel and user, and writes the summary to an output file.
     *
     * @param channelName The name of the channel.
     * @param user The user for whom the summary is generated.
     * @param outputFile The file to write the summary to.
     */
    void summarize(string &channelName, string &user, string &outputFile);

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

