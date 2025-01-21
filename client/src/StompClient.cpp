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

	Frame connectFrame("CONNECT");
	connectFrame.addHeader("accept-version", "1.2");
	connectFrame.addHeader("host", host_);
	connectFrame.addHeader("login", username_);
	connectFrame.addHeader("passcode", password_);

	sendFrame(connectFrame.toString());
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
	Frame frameToSend("SEND");
	frameToSend.addHeader("destination", destination);
	frameToSend.setBody(body);

	sendFrame(frameToSend.toString());
	Frame response = receiveFrame();

	if (response.getCommand() == "ERROR")
	{
		// todo: handle error
	}
}

void StompClient::subscribe(string &destination)
{
	if (channelToSubscriptionID_.find(destination) != channelToSubscriptionID_.end())
	{
		cout << "Client is already subscribed to channel " << destination << endl;
		return;
	}

	Frame subscribeFrame("SUBSCRIBE");
	subscribeFrame.addHeader("destination", destination);
	subscribeFrame.addHeader("id", to_string(nextSubscriptionID_));

	sendFrame(subscribeFrame.toString());
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

	Frame unsubscribeFrame("UNSUBSCRIBE");
	unsubscribeFrame.addHeader("id", to_string(channelToSubscriptionID_[destination]));

	sendFrame(unsubscribeFrame.toString());
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
	Frame disconnectFrame("DISCONNECT");

	//todo: replace placeholder receipt number
	disconnectFrame.addHeader("receipt", to_string(1111));

	sendFrame(disconnectFrame.toString());
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
	return Frame::parseFrame(frameString);
}

main(int argc, char *argv[])
{
	// TODO: implement the STOMP client
	return 0;
}