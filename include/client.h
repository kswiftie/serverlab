#define _WIN32_WINNT_WIN10 0x0A00

#include <iostream>
#include <string>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <inaddr.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <cctype>
#include <functional>
#include <sstream>

const short BUFFER_SIZE = 4096;

class Client {
private:
	int erStat;
	SOCKET ClientSocket;
public:
	Client();
	void connect_to_server(const short server_port, std::string server_ip);
	void send_request(std::string server_ip, short server_port, std::string request);
	void stop();
	std::string get_request(std::string request, std::string host, std::string port);
	void inp_request(std::string host, short port);
	void tolower_str(std::string& s);
};

class Request {
private:
    std::string server_id;
	std::string s;
    std::string method;
    std::string body;
//public:
	//Request() : {};
};