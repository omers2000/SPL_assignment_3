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
    int port_;
    bool loggedIn_;

    string username_;
    string password_;
    int nextSubscriptionID_;
    unordered_map<string, int> channelToSubscriptionID_;
    unordered_map<string, vector<Event>> channelToEvents_;

    void sendFrame(const string &frame);
    Frame receiveFrame();


public:
    StompClient();

    ~StompClient();

    void connect(string &host, short port, string &username, string &password);

    void send(string &destination, string &body);

    void report(string &filePath);

    void subscribe(string &destination);

    void unsubscribe(string &destination);

    void disconnect();

    void summarize(string &channelName, string &user, string &outputFile);

};