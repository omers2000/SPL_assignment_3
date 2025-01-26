CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread
all: StompEMIClient

EchoClient: .PHONY bin/ConnectionHandler.o bin/echoClient.o
	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompEMIClient: .PHONY bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/Frame.o bin/clientUtils.o bin/StompProtocol.o
	g++ -o bin/StompEMIClient bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/Frame.o bin/StompProtocol.o  bin/clientUtils.o $(LDFLAGS)

bin/ConnectionHandler.o: 
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: 
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: 
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompProtocol.o: 
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/clientUtils.o: 
	g++ $(CFLAGS) -o bin/clientUtils.o src/clientUtils.cpp

bin/StompClient.o: 
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/Frame.o: 
		g++ $(CFLAGS) -o bin/Frame.o src/Frame.cpp

.PHONY: clean
clean:
	rm -f bin/*
	
