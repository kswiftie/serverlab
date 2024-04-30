#define _WIN32_WINNT_WIN10 0x0A00

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <cstdint>
#include <vector>
#include <thread>
#include <chrono>

const short BUFF_SIZE = 1024;

class Server {
private:
	int erStat; // Keeps socket errors status
    SOCKET ServSock;
	uint16_t port;
	bool status;
public:
	Server(std::string server_ip, uint16_t port_value);
	~Server();
	// void start(); probably it needed
	void client_handler(SOCKET client, sockaddr_in clientInfo);
	void waitingAcceptLoop();
	void stop();
};