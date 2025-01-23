#include "../include/StompClient.h"
#include <fstream>
#include "StompClient.h"

// Stomp client implementation:
int main(int argc, char *argv[])
{
	StompClient client;

	std::thread userInputThread(handleUserInput, std::ref(client));
	std::thread receiveThread(&StompClient::receive, &client);
	//todo: implement synchonization
	userInputThread.join();
	receiveThread.join();
	
}

//todo: consider turning this into a non-static class method
void handleUserInput(StompClient &client)
{
	string command;
	vector<string> commandArgs;
	while (true)
	{
		cout << "Enter command and arguments (send, subscribe, unsubscribe, report, summarize, disconnect, exit): ";
		cin >> command;
		split_str(command, ' ', commandArgs);

		if (commandArgs[0] == "login")
		{
			if (commandArgs.size() < 5)
			{
				cout << "Usage: login <host> <port> <username> <password>" << endl;
			}
			else
			{
				string hostPortString = commandArgs[1];
				size_t colonPos = hostPortString.find(':');
				if (colonPos == string::npos)
				{
					cout << "Invalid host:port format" << endl;
					continue;
				}
				string host = hostPortString.substr(0, colonPos);
				short port = stoi(hostPortString.substr(colonPos + 1));

				string username = commandArgs[2];
				string password = commandArgs[3];
				client.connect(host, port, username, password);
			}
		}
		if (client.isLoggedIn() == false)
		{
			cout << "You must login first" << endl;
			continue;
		}

		else if (commandArgs[0] == "join")
		{
			if (commandArgs.size() < 2)
			{
				cout << "Usage: join <destination>" << endl;
			}
			else
			{
				string destination = commandArgs[1];
				client.subscribe(destination);
			}
		}
		else if (commandArgs[0] == "exit")
		{
			if (commandArgs.size() < 2)
			{
				cout << "Usage: exit <destination>" << endl;
			}
			else
			{
				string destination = commandArgs[1];
				client.unsubscribe(destination);
			}
		}
		else if (commandArgs[0] == "report")
		{
			if (commandArgs.size() < 2)
			{
				cout << "Usage: report <file path>" << endl;
			}
			else
			{
				string filePath = commandArgs[1];
				client.report(filePath);
			}
		}
		else if (commandArgs[0] == "summary")
		{
			if (commandArgs.size() < 4)
			{
				cout << "Usage: summary <channel name> <user> <output file path>" << endl;
			}
			else
			{
				string channelName = commandArgs[1];
				string user = commandArgs[2];
				string outputFile = commandArgs[3];
				client.summarize(channelName, user, outputFile);
			}
		}
		else if (commandArgs[0] == "logout")
		{
			client.disconnect();
			break;
		}

		else
		{
			cout << "Unknown command" << endl;
		}

		commandArgs.clear();
	}
}

void StompClient::receive()
{
	while (true)
	{
		Frame frame = receiveFrame();
		if (frame.getCommand() == "MESSAGE")
		{
			string destination = frame.getHeader("destination");
			if (channelToEvents_.find(destination) != channelToEvents_.end())
			{
				Event event = Event(frame.getBody());
				channelToEvents_[destination].push_back(event);
				cout << "Received event on channel " << destination << ": " << event.get_name() << endl;
			}
			else
			{
				cerr << "Received message for unknown channel " << destination << endl;
			}
		}
		else if (frame.getCommand() == "ERROR")
		{
			cerr << "Received error frame: " << frame.getBody() << endl;
		}
	}
}

StompClient::StompClient() : loggedIn_(false), nextSubscriptionID_(0), nextReceiptID_(0), connectionHandler_(nullptr) {}

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

	// TODO: deal with the case where the user is not subscribed to the channel
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

	if (response.getCommand() == "ERROR")
	{
		// todo: handle error types: User is already subscribed to this channel, etc.
	}

	cout << "Joined Channel " << destination << endl;
	channelToSubscriptionID_[destination] = nextSubscriptionID_;
	nextSubscriptionID_++;
	channelToEvents_[destination] = vector<Event>();
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

	if (response.getCommand() == "ERROR")
	{
		// todo: handle error
	}

	cout << "Exited Channel " << destination << endl;
	// todo: decide whether to delete the channel from the maps or not
	channelToSubscriptionID_.erase(destination);
	channelToEvents_.erase(destination);
}

void StompClient::report(string &filePath)
{
	try
	{
		names_and_events names_and_events = parseEventsFile(filePath);
		for (Event event : names_and_events.events)
		{
			string eventString = event.toString();
			send(names_and_events.channel_name, eventString);
			// todo: check if the event was sent successfully
			channelToEvents_[names_and_events.channel_name].push_back(event);
		}
	}
	catch (const std::exception &e)
	{
		// todo: handle error properly
		cerr << "Couldn't read file: " << e.what() << endl;
	}
}

void StompClient::disconnect()
{
	if (!loggedIn_)
	{
		cout << "The client is not logged in." << endl;
		return;
	}

	Frame disconnectFrame("DISCONNECT");
	disconnectFrame.addHeader("receipt", username_ + "-" + to_string(nextReceiptID_));

	sendFrame(disconnectFrame.toString());
	Frame response = receiveFrame();

	if (response.getCommand() == "RECEIPT")
	{
		cout << "Disconnected from server" << endl;
		connectionHandler_->close();
		nextReceiptID_++;
	}
	else if (response.getCommand() == "ERROR")
	{
		// todo: handle error
	}
}

void StompClient::summarize(string &channelName, string &user, string &outputFile)
{
	if (channelToEvents_.find(channelName) == channelToEvents_.end())
	{
		cerr << "Channel " << channelName << " not found." << endl;
		return;
	}

	vector<Event> events = channelToEvents_[channelName];
	vector<Event> userEvents;

	// Filter events by user
	for (const auto &event : events)
	{
		if (event.getEventOwnerUser() == user)
		{
			userEvents.push_back(event);
		}
	}

	if (userEvents.empty())
	{
		cerr << "No events found for user " << user << " in channel " << channelName << "." << endl;
		return;
	}

	// Aggregate event information
	int totalReports = userEvents.size();
	int activeCount = 0;
	int forcesArrivalCount = 0;

	for (const auto &event : userEvents)
	{
		if (event.get_general_information().at("active") == "true")
		{
			activeCount++;
		}
		if (event.get_general_information().at("forces arrival at scene") == "true")
		{
			forcesArrivalCount++;
		}
	}

	// Write summary to output file
	ofstream outFile(outputFile);
	if (!outFile.is_open())
	{
		cerr << "Could not open file " << outputFile << " for writing." << endl;
		return;
	}

	outFile << "Channel " << channelName << "\n";
	outFile << "Stats:\n";
	outFile << "Total: " << totalReports << "\n";
	outFile << "active: " << activeCount << "\n";
	outFile << "forces arrival at scene: " << forcesArrivalCount << "\n";
	outFile << "Event Reports:\n";

	// todo: ensure that Reports are sorted by date_time by time and after that by event_name lexicographically.
	int reportNumber = 1;
	for (const auto &event : userEvents)
	{
		outFile << "Report_" << reportNumber++ << ":\n";
		outFile << "city: " << event.get_city() << "\n";
		outFile << "date time: " << event.get_date_time() << "\n";
		outFile << "event name: " << event.get_name() << "\n";
		outFile << "summary: " << event.get_description() << "\n";
		outFile << "\n";
	}

	outFile.close();
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

// Function to split a string by a delimiter
void split_str(const std::string &s, char delimiter, std::vector<std::string> &tokens)
{
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
}

bool StompClient::isLoggedIn() const
{
	return loggedIn_;
}

string StompClient::getHost() const
{
	return host_;
}

short StompClient::getPort() const
{
	return port_;
}

string StompClient::getUsername() const
{
	return username_;
}

string StompClient::getPassword() const
{
	return password_;
}

int StompClient::getNextSubscriptionID() const
{
	return nextSubscriptionID_;
}

int StompClient::getNextReceiptID() const
{
	return nextReceiptID_;
}

ConnectionHandler *StompClient::getConnectionHandler() const
{
	return connectionHandler_;
}
