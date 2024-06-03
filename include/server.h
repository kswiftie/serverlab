#define _WIN32_WINNT_WIN10 0x0A00

#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <string>
#include <regex>
#include <functional>
#include <map>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <functional>
#include <thread>

const short BUFF_SIZE = 4096;
std::regex listen_song_by_id("GET /HTTP/127.0.0.1:8080/listen_song\\?song_id=\\d+$");
std::regex listen_album_by_id("GET /HTTP/127.0.0.1:8080/listen_album\\?album_id=\\d+$");
std::regex listen_song_by_info("\
GET /listen_song /HTTP/1.1\n\
Host: 127.0.0.1:8080\n\
song=([!-~]+)&album=([!-~]+)&artist=([!-~]+)");
std::regex listen_album_by_info("\
GET /listen_album /HTTP/1.1\n\
Host: 127.0.0.1:8080\n\
album=([!-~]+)&artist=([!-~]+)");
std::string help = "GET /HTTP/127.0.0.1:8080/help";
std::regex login("\
POST /login HTTP/1.1\n\
Host: 127.0.0.1:8080\n\
username=(\\w+)&password=([!-~]+)$");
std::string masklogout = "GET /HTTP/127.0.0.1:8080/logout";
std::regex reg("\
POST /register HTTP/1.1\n\
Host: 127.0.0.1:8080\n\
username=(\\w+)&password=([!-~]+)$");

class Server {
private:
	int erStat; // Keeps socket errors status
    SOCKET ServSock;
	uint16_t port;
	bool status;
	std::map <std::string, std::string> loggedin_users;
public:
	Server(std::string server_ip, uint16_t port_value);
	~Server();
	std::string get_response(std::string request, std::string userip);
	void waitingAcceptLoop();
	void response(SOCKET client, sockaddr_in clientInfo);
	std::string reg_user(std::string name, std::string password, std::string userip);
	std::string login_user(std::string name, std::string password, std::string userip);
	std::string logout(std::string userip);
	std::string listen_song(std::string song,
std::string album,
std::string artist, std::string clientip);
	std::string gethelp();
	std::vector<std::string> get_id(std::string name);
	std::string listen_album(std::string album, std::string artist, std::string clientip);
	void update_song_info(std::string song_id, std::string client);
	void stop();
};