CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread
all: StompEMIClient

EchoClient: bin/ConnectionHandler.o bin/echoClient.o
	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompEMIClient: bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/Frame.o bin/ClientUtils.o bin/StompProtocol.o
	g++ -o bin/StompEMIClient bin/ConnectionHandler.o bin/StompClient.o bin/event.o bin/Frame.o bin/StompProtocol.o  bin/ClientUtils.o $(LDFLAGS)

bin/ConnectionHandler.o: 
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: 
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: 
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompProtocol.o: 
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/ClientUtils.o: 
	g++ $(CFLAGS) -o bin/ClientUtils.o src/ClientUtils.cpp

bin/StompClient.o: 
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/Frame.o: 
		g++ $(CFLAGS) -o bin/Frame.o src/Frame.cpp

.PHONY: clean
clean:
	rm -f bin/*
	