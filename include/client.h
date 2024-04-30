#define _WIN32_WINNT_WIN10 0x0A00

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <vector>

const short BUFFER_SIZE = 1024;

class Client {
private:
	int erStat;
	SOCKET ClientSocket;
public:
	Client();
	void connect_to_server(const short server_port, std::string server_ip);
};