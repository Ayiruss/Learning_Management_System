#make file for Server
MAKE = make
RM =rm
all: server.cpp distributedClient.cpp
	g++ -o server server.cpp -l sqlite3
	g++ -o client distributedClient.cpp 