#include "..\\include\\server.h"
#include <functional>

Server::Server(std::string server_ip, uint16_t port_value) {
	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	this->erStat = inet_pton(AF_INET, server_ip.c_str(), &ip_to_num);
	
	if (this->erStat <= 0) {
		std::cout << "Error in IP translation to special numeric format\n";
		exit(1);
	}
    
	// Server socket initialization
	this->ServSock = socket(AF_INET, SOCK_STREAM, 0);

	if (this->ServSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << "\n";
		closesocket(this->ServSock);
		WSACleanup();
		exit(1);
	}
	else
		std::cout << "Server socket initialization is OK\n";

	// Server socket binding
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure
				
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;	
	servInfo.sin_port = htons(port_value);

	this->erStat = bind(this->ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (this->erStat != 0) {
		std::cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << "\n";
		closesocket(this->ServSock);
		WSACleanup();
		exit(1);
	}
	else 
		std::cout << "Binding socket to Server info is OK\n";
}

void Server::stop() {
	closesocket(this->ServSock);
	WSACleanup();
}

Server::~Server() {
  if(this->status)
    stop();
}

void Server::waitingAcceptLoop() {
	while (this->status) {
		//Starting to listen to any Clients
		this->erStat = listen(this->ServSock, SOMAXCONN);

		if (this->erStat != 0) {
			std::cout << "Can't start to listen to. Error # " << WSAGetLastError() << "\n";
			stop();
		}
		std::cout << "Listening...";
		
		std::vector<std::thread> clientThreads;

		sockaddr_in clientInfo;
		ZeroMemory(&clientInfo, sizeof(clientInfo));
		int clientInfo_size = sizeof(clientInfo);
		SOCKET clientSocket = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

		if (clientSocket < 0) {
			std::cerr << "Accept error" << WSAGetLastError() << "\n";
			continue;
		}
		if (clientThreads.size() < 2)
			clientThreads.emplace_back(std::bind(&Server::client_handler, this, clientSocket, clientInfo));

		for (auto& thread : clientThreads) {
			thread.join();
		}
		clientThreads.clear();
	}
}

void Server::client_handler(SOCKET client, sockaddr_in clientInfo) {
	//Client socket creation and acception in case of connection
	// sockaddr_in clientInfo;
	// ZeroMemory(&clientInfo, sizeof(clientInfo)); // Initializing clientInfo structure

	// int clientInfo_size = sizeof(clientInfo);

	// SOCKET client = accept(this->ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

	if (client == INVALID_SOCKET) {
		std::cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << "\n";
		closesocket(client);
		stop();
		exit(1);
	}
	else {
		std::cout << "Connection to a client established successfully\n";
		char clientIP[22];
		inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN); // Convert connected client's IP to standard string format
		std::cout << "Client connected with IP address " << clientIP << "\n";
	}
	
	//Exchange text data between Server and Client. Disconnection if a client send "xxx"
	std::vector <char> servBuff(BUFF_SIZE);
	std::vector <char> clientBuff(BUFF_SIZE); // Creation of buffers for sending and receiving data
	short packet_size = 0; // The size of sending / receiving packet in bytes
	
	while (true) {
		packet_size = recv(client, servBuff.data(), servBuff.size(), 0); // Receiving packet from client. Program is waiting (system pause) until receive
		std::cout << client << " Client's message: " << servBuff.data() << "\n";

		std::cout << "Your (host) message: ";
		fgets(clientBuff.data(), clientBuff.size(), stdin);

		// Check whether server would like to stop chatting 
		if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
			shutdown(client, SD_BOTH);
			closesocket(client);
			break;
		}

		packet_size = send(client, clientBuff.data(), clientBuff.size(), 0);

		if (packet_size == SOCKET_ERROR) {
			std::cout << "Can't send message to Client. Error # " << WSAGetLastError() << "\n";
			closesocket(client);
			stop();
			exit(1);
		}
	}
	closesocket(client);
}

int main() {
	// запустим wsastartup
	WSADATA wsaData;
    int wsastart = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (wsastart != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << "\n";
		WSACleanup();
        return wsastart;
    }
	std::cout << "WSAStartup was succesful\n";

    Server server("127.0.0.1", 6666);
	server.waitingAcceptLoop();
}