#include "../include/StompClient.h"
#include "../include/clientUtils.h"
#include <fstream>
#include "StompClient.h"

void handleUserInput(StompClient &client)
{
	cout << "User input thread started" << endl;
	string command;
	vector<string> commandArgs;
	while (true)
	{
		cout << "Enter command and arguments (login, join, exit, report, summary, logout):" << endl;
		getline(cin, command);
		commandArgs.clear();
		ClientUtils::split_str(command, ' ', commandArgs);

		if (commandArgs[0] == "login")
		{
			if (commandArgs.size() < 4)
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
				client.login(host, port, username, password);
			}
		}
		else if (client.isLoggedIn() == false)
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
				client.join(destination);
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
				client.exit(destination);
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
				client.summary(channelName, user, outputFile);
			}
		}
		else if (commandArgs[0] == "logout")
		{
			client.logout();
		}

		else
		{
			cout << "Unknown command" << endl;
		}
	}
}

void StompClient::listen()
{
	cout << "Listener thread started" << endl;

	while (true)
	{
		unique_lock<mutex> lock(loginLock_);
		loginCV_.wait(lock, [this]
					  { return loggedIn_; });

		Frame frame = stompProtocol_.receiveFrame();
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
		else
		{
			// Store last response (not a MESSAGE frame)
			{
				// unique_lock<mutex> lock(responseLock_);
				lastResponse_ = frame;
				lastResponseUpdated_ = true;
				responseCV_.notify_all(); // Wake up waiting threads

				if (frame.getCommand() == "ERROR")
				{
					loggedIn_ = false;
					loginCV_.notify_all(); // Notify the condition variable
				}
			}
		}
	}
}

Frame StompClient::getResponse()
{
	unique_lock<mutex> lock(responseLock_);

	// Wait until lastResponse_ is set (not empty)
	responseCV_.wait(lock, [this]
					 { return lastResponseUpdated_ == true; });

	Frame response = lastResponse_; // Take the response
	lastResponseUpdated_ = false;	// Reset after taking
	return response;
}

StompClient::StompClient() : stompProtocol_(), loggedIn_(false), username_(), password_(), nextSubscriptionID_(0), nextReceiptID_(0),
							 channelToSubscriptionID_(), channelToEvents_(),
							 responseLock_(), responseCV_(), lastResponse_("EMPTY"), lastResponseUpdated_(false),
							 loginLock_(), loginCV_()
{
}

StompClient::~StompClient()
{
	stompProtocol_.disconnect();
}

void StompClient::login(string &host, short port, string &username, string &password)
{
	if (loggedIn_)
	{
		cout << "The client is already logged in, log out before trying again." << endl;
		return;
	}

	if (!stompProtocol_.connect(host, port, username, password))
	{
		cerr << "Could not connect to server" << endl;
		return;
	}

	Frame connectFrame("CONNECT");
	connectFrame.addHeader("accept-version", "1.2");
	connectFrame.addHeader("host", "stomp.cs.bgu.ac.il");
	connectFrame.addHeader("login", username);
	connectFrame.addHeader("passcode", password);

	stompProtocol_.sendFrame(connectFrame.toString());
	Frame response = stompProtocol_.receiveFrame();

	if (response.getCommand() == "CONNECTED")
	{
		username_ = username;
		password_ = password;
		loggedIn_ = true;
		loginCV_.notify_all();
		cout << "Login successful" << endl;
	}
	else if (response.getCommand() == "ERROR")
	{
		cout << "Login failed" << endl;
		handleError(response);
	}
}

void StompClient::logout()
{
	if (!loggedIn_)
	{
		cout << "The client is not logged in." << endl;
		return;
	}

	Frame disconnectFrame("DISCONNECT");
	disconnectFrame.addHeader("receipt", generateNextReceiptID());

	stompProtocol_.sendFrame(disconnectFrame.toString());
	Frame response = getResponse();

	if (response.getCommand() == "RECEIPT")
	{
		cout << "Disconnected from server" << endl;
		// todo: decide whether to delete the data from the client or not
		loggedIn_ = false;
		loginCV_.notify_all();
		stompProtocol_.disconnect();
	}
}

void StompClient::join(string &destination)
{
	if (channelToSubscriptionID_.find(destination) != channelToSubscriptionID_.end())
	{
		cout << "Client is already subscribed to channel " << destination << endl;
		return;
	}

	Frame subscribeFrame("SUBSCRIBE");
	subscribeFrame.addHeader("destination", destination);
	subscribeFrame.addHeader("id", generateNextSubscriptionID());
	subscribeFrame.addHeader("receipt", generateNextReceiptID());

	stompProtocol_.sendFrame(subscribeFrame.toString());
	Frame response = getResponse();

	if (response.getCommand() == "RECEIPT")
	{
		cout << "Joined Channel " << destination << endl;
		channelToSubscriptionID_[destination] = nextSubscriptionID_;
		channelToEvents_[destination] = vector<Event>();
	}

	else if (response.getCommand() == "ERROR")
	{
		cout << "Error joining channel " << destination << endl;
		handleError(response);
	}
}

void StompClient::exit(string &destination)
{
	if (channelToSubscriptionID_.find(destination) == channelToSubscriptionID_.end())
	{
		cout << "Client is not subscribed to channel " << destination << endl;
		return;
	}

	Frame unsubscribeFrame("UNSUBSCRIBE");
	unsubscribeFrame.addHeader("id", to_string(channelToSubscriptionID_[destination]));
	unsubscribeFrame.addHeader("receipt", generateNextReceiptID());

	stompProtocol_.sendFrame(unsubscribeFrame.toString());
	Frame response = getResponse();

	if (response.getCommand() == "RECEIPT")
	{
		cout << "Exited Channel " << destination << endl;
		// todo: decide whether to delete the channel from the maps or not
		channelToSubscriptionID_.erase(destination);
		channelToEvents_.erase(destination);
	}
	else if (response.getCommand() == "ERROR")
	{
		cout << "Error exiting channel " << destination << endl;
		cout << response.toString() << endl;
		loggedIn_ = false;
		stompProtocol_.disconnect();
	}
}

void StompClient::report(string &filePath)
{
	try
	{
		names_and_events names_and_events = parseEventsFile(filePath);
		if (!names_and_events.events.empty())
		{
			for (Event event : names_and_events.events)
			{
				if (!loggedIn_)
				{
					cout << "The client was disconnected, couldn't report " << filePath << endl;
					return;
				}

				event.setEventOwnerUser(username_);
				string eventString = event.toString();
				send(names_and_events.channel_name, eventString);
				channelToEvents_[names_and_events.channel_name].push_back(event);
			}
			cout << "Reports sent successfully" << endl;
		}
		else
		{
			cout << "No events to report in" << filePath << endl;
		}
	}
	catch (const std::exception &e)
	{
		cerr << "Couldn't read file: " << e.what() << "\nenter full file path" << endl;
	}
}

void StompClient::send(string &destination, string &body)
{
	Frame frameToSend("SEND");
	frameToSend.addHeader("destination", destination);
	frameToSend.addHeader("receipt", generateNextReceiptID());
	frameToSend.setBody(body);

	// TODO: deal with the case where the user is not subscribed to the channel
	stompProtocol_.sendFrame(frameToSend.toString());
	Frame response = getResponse();

	if (response.getCommand() == "ERROR")
	{
		cout << "Error sending message" << endl;
		handleError(response);
	}
}

void StompClient::summary(string &channelName, string &user, string &outputFile)
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

	// Aggregate event information
	int totalReports = userEvents.size();
	int activeCount = 0;
	int forcesArrivalCount = 0;

	for (const auto &event : userEvents)
	{
		auto generalInfo = event.get_general_information();
		if (generalInfo["active"] == "true")
		{
			activeCount++;
		}
		if (generalInfo["forces arrival at scene"] == "true")
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
	outFile << "forces arrival at scene: " << forcesArrivalCount << "\n\n";
	outFile << "Event Reports:\n";

	// Sort userEvents by date_time and then by event_name lexicographically
	std::sort(userEvents.begin(), userEvents.end(), [](const Event &a, const Event &b)
			  {
		if (a.get_date_time() == b.get_date_time()) {
			return a.get_name() < b.get_name();
		}
		return a.get_date_time() < b.get_date_time(); });

	int reportNumber = 1;
	for (const auto &event : userEvents)
	{
		outFile << "Report_" << reportNumber++ << ":\n";
		outFile << "city: " << event.get_city() << "\n";
		outFile << "date time: " << ClientUtils::epochToDateTimeString(event.get_date_time()) << "\n";
		outFile << "event name: " << event.get_name() << "\n";
		string description = event.get_description();
		if (description.length() > 27)
		{
			description = description.substr(0, 27) + "...";
		}
		outFile << "summary: " << description << "\n";
		outFile << "\n";
	}
	cout << "Summary written to " << outputFile << endl;
	outFile.close();
}

bool StompClient::isLoggedIn() const
{
	return loggedIn_;
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

string StompClient::generateNextSubscriptionID()
{
	string nextSubscriptionIDStr = to_string(nextSubscriptionID_);
	nextSubscriptionID_++;
	return nextSubscriptionIDStr;
}

int StompClient::getNextReceiptID() const
{
	return nextReceiptID_;
}

string StompClient::generateNextReceiptID()
{
	string nextReceiptIDStr = to_string(nextReceiptID_);
	nextReceiptID_++;
	return nextReceiptIDStr;
}

void StompClient::handleError(Frame &response)
{
	cout << response.toString() << endl;
	// todo: decide whether to delete the data from the client or not
	loggedIn_ = false;
	loginCV_.notify_all();
	stompProtocol_.disconnect();
}

// Stomp client implementation:
int main(int argc, char *argv[])
{
	StompClient client;

	thread receiveThread(&StompClient::listen, &client);
	handleUserInput(client);
}