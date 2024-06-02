#include "..\\include\\client.h"

Client::Client() {
		this->erStat; // For checking errors in sockets functions

		WSADATA wsData;
		this->erStat = WSAStartup(MAKEWORD(2,2), &wsData);

		if (this->erStat != 0) {
			std::cout << "Error WinSock version initializaion #";
			std::cout << WSAGetLastError();
			exit(1);
		}
	}

void Client::send_request(std::string host, short port, std::string request) {
    this->ClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (this->ClientSocket == INVALID_SOCKET) {
        std::cout << "Error initialization socket # " << WSAGetLastError() << "\n";
        closesocket(this->ClientSocket);
    }
    in_addr ip_to_num;
    inet_pton(AF_INET, host.c_str(), &ip_to_num);

    // Establishing a connection to Server
    sockaddr_in server_info;
    ZeroMemory(&server_info, sizeof(server_info));

    server_info.sin_family = AF_INET;
    server_info.sin_addr = ip_to_num;	
    server_info.sin_port = htons(port);
    this->erStat = connect(this->ClientSocket, (sockaddr*)&server_info, sizeof(server_info));
    
    if (this->erStat != 0) {
        std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << "\n";
        return;
    }

    char serverBuff[BUFFER_SIZE];
    short packet_size = 0;

    packet_size = send(this->ClientSocket, request.c_str(), BUFFER_SIZE, 0);

    if (packet_size == SOCKET_ERROR) {
        std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << "\n";
        return;
    }
    
    packet_size = recv(this->ClientSocket, serverBuff, BUFFER_SIZE, 0);

    if (packet_size == SOCKET_ERROR) {
        std::cout << "Can't receive message from Server. Error # " << WSAGetLastError() << "\n";
        return;
    }
    std::cout << serverBuff << "\n";
    memset(&serverBuff[0], 0, sizeof(serverBuff));
    closesocket(this->ClientSocket);
}

void Client::inp_request(std::string host, short port) {
    std::string request = "";
    std::string string_port = std::to_string(port);
    while (request != "stop") {
        std::cout << "Enter your request: ";
        std::getline(std::cin, request); tolower_str(request);
        send_request(host, port, get_request(request, host, string_port));
    }
}

std::string Client::get_request(std::string request, std::string host, std::string port) {
    std::string res = "GET /HTTP/" + host + (std::string)":" + port;
    if (request == "login") {
        std::string login, password;
        std::cout << "Type login: ";
        std::getline(std::cin, login);
        std::cout << "Type password: ";
        std::getline(std::cin, password);
        res = "\
POST /login HTTP/1.1\n\
Host: " + host + (std::string)":" + port + "\n\
username=" + login + "&password=" + password;
    }
    else if (request == "logout") {
        res = "GET /HTTP/" + host + ":" + port + "/logout";
    }
    else if (request == "register") {
        std::string login, password;
        std::cout << "Type login: ";
        std::getline(std::cin, login);
        std::cout << "Type password: ";
        std::getline(std::cin, password);
        res = "\
POST /register HTTP/1.1\n\
Host: " + host + (std::string)":" + port + "\n\
username=" + login + "&password=" + password;
    }
    else if (request == "help") {
        res = "GET /HTTP/" + host + ":" + port + "/help";
    }
    else if (request == "listen song") {
        std::cout << "Enter the name of the song: ";
        std::string inp;
        std::getline(std::cin, inp); tolower_str(inp);
        std::string song = "", album = "", artist = "";
        std::istringstream splitrequest(inp);
        splitrequest >> song;
        splitrequest >> album;
        splitrequest >> artist;
        res = "GET /listensong /HTTP/1.1\n\
Host: " + host + (std::string)":" + port + "\n" +
"song=" + song +
"&album=" + album +
"&artist=" + artist;
    }
    else if (request == "listen album") {
        std::cout << "Enter the name of the album: ";
        std::getline(std::cin, request); tolower_str(request);
        res = "GET /HTTP/" + host + port +"/listen_album?song_info=" + request;
    }
    return res;
}

void Client::tolower_str(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
}

int main() {
    system("chcp 1251 >NUL 2>&1");
	WSADATA wsaData;
    int wsastart = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsastart != 0) {
        std::cerr << "WSAStartup failed: " << wsastart << "\n";
		WSACleanup();
        return wsastart;
    }

    std::string host = "127.0.0.1"; short port = 8080;
    Client client;
    client.inp_request(host, port);
    WSACleanup();
    return 0;
}