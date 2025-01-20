#include "../include/StompClient.h"
#include "StompClient.h"

// Stomp client implementation:

StompClient::StompClient() : loggedIn_(false), nextSubscriptionID_(0) {}

StompClient::~StompClient()
{
	if (connectionHandler_ != nullptr)
	{
		connectionHandler_->close();
		delete connectionHandler_;
		connectionHandler_ = nullptr;
	}
}

void StompClient::connect(string &host, short port, string &username, string &password)
{
	if (loggedIn_)
	{
		cout << "The client is already logged in, log out before trying again." << endl;
		return;
	}

	host_ = host;
	port_ = port;
	username_ = username;
	password_ = password;

	connectionHandler_ = new ConnectionHandler(host_, port_);
	if (!connectionHandler_->connect())
	{
		cerr << "Could not connect to server" << endl;
		delete connectionHandler_;
		connectionHandler_ = nullptr;
		return;
	}

	string connectFrame = "CONNECT\n";
	connectFrame += "accept-version:1.2\n";
	connectFrame += "host:" + host_ + "\n";
	connectFrame += "login:" + username_ + "\n";
	connectFrame += "passcode:" + password_ + "\n\n";
	connectFrame += '\0';

	sendFrame(connectFrame);
	Frame response = receiveFrame();

	if (response.getCommand() == "CONNECTED")
	{
		loggedIn_ = true;
		cout << "Login successful" << endl;
	}
	else if (response.getCommand() == "ERROR")
	{
		// todo: handle error
	}
}

void StompClient::send(string &destination, string &body)
{
	string frameToSend = "SEND\n";
	frameToSend += "destination:" + destination + "\n\n";
	frameToSend += body + "\n";

	sendFrame(frameToSend);
	Frame response = receiveFrame();

	if (response.getCommand() == "ERROR")
	{
		// todo: handle error
	}
}

void StompClient::subscribe(string &destination)
{
	string subscribeFrame = "SUBSCRIBE\n";
	subscribeFrame += "destination:" + destination + "\n";
	subscribeFrame += "id:" + to_string(nextSubscriptionID_) + "\n\n";

	sendFrame(subscribeFrame);
	Frame response = receiveFrame();

	if (response.getCommand() == "RECEIPT")
	{
		cout << "Joined Channel " << destination << endl;
		channelToSubscriptionID_[destination] = nextSubscriptionID_;
		nextSubscriptionID_++;
		channelToEvents_[destination] = vector<Event>();
	}
	else if (response.getCommand() == "ERROR")
	{
		// todo: handle error types: User is already subscribed to this channel, etc.
	}
}

void StompClient::unsubscribe(string &destination)
{
	if (channelToSubscriptionID_.find(destination) == channelToSubscriptionID_.end())
	{
		cout << "Client is not subscribed to channel " << destination << endl;
		return;
	}

	string unsubscribeFrame = "SUBSCRIBE\n";
	unsubscribeFrame += "id:" + to_string(channelToSubscriptionID_[destination]) + "\n\n";

	sendFrame(unsubscribeFrame);
	Frame response = receiveFrame();

	if (response.getCommand() == "RECEIPT")
	{
		cout << "Exited Channel " << destination << endl;

		// todo: decide whether to delete the channel from the maps or not
		channelToSubscriptionID_.erase(destination);
		channelToEvents_.erase(destination);
	}
	else if (response.getCommand() == "ERROR")
	{
		// todo: handle error types: User is already subscribed to this channel, etc.
	}
}

void StompClient::report(string &filePath)
{
	// Report implementation
}

void StompClient::disconnect()
{
	string disconnectFrame = "DISCONNECT\n";
	disconnectFrame += "receipt:" + to_string(1111) + "\n\n";

	sendFrame(disconnectFrame);
	Frame response = receiveFrame();

	if (response.getCommand() == "RECEIPT")
	{
		cout << "Disconnected from server" << endl;
		connectionHandler_->close();
	}
	else if (response.getCommand() == "ERROR")
	{
		// todo: handle error
	}
}

void StompClient::summarize(string &channelName, string &user, string &outputFile)
{
	// Summarize implementation
}

void StompClient::sendFrame(const string &frame)
{
	connectionHandler_->sendFrameAscii(frame, '\0');
}

Frame StompClient::receiveFrame()
{
	string frameString;
	connectionHandler_->getFrameAscii(frameString, '\0');
	return parseFrame(frameString);
}

Frame parseFrame(const string &frameString)
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

vector<string> splitIntoLines(const string &input)
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

main(int argc, char *argv[])
{
	// TODO: implement the STOMP client

	return 0;
}