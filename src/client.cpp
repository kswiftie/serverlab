#include "..\\include\\client.h"

Client::Client() {
		//Key constants

		// Key variables for all program
		this->erStat; // For checking errors in sockets functions

		// WinSock initialization
		WSADATA wsData;
		this->erStat = WSAStartup(MAKEWORD(2,2), &wsData);

		if (this->erStat != 0) {
			std::cout << "Error WinSock version initializaion #";
			std::cout << WSAGetLastError();
			exit(1);
		}
		else 
			std::cout << "WinSock initialization is OK\n";
		
		// Socket initialization
		this->ClientSocket = socket(AF_INET, SOCK_STREAM, 0);

		if (this->ClientSocket == INVALID_SOCKET) {
			std::cout << "Error initialization socket # " << WSAGetLastError() << "\n";
			closesocket(this->ClientSocket);
			WSACleanup();
		}
		else 
			std::cout << "Client socket initialization is OK\n";	
	}

void Client::connect_to_server(const short server_port, std::string server_ip) {
    //IP in string format to numeric format for socket functions. Data is in "ip_to_num"
    in_addr ip_to_num;
    inet_pton(AF_INET, server_ip.c_str(), &ip_to_num);

    // Establishing a connection to Server
    sockaddr_in server_info;
    ZeroMemory(&server_info, sizeof(server_info));

    server_info.sin_family = AF_INET;
    server_info.sin_addr = ip_to_num;	
    server_info.sin_port = htons(server_port);
    this->erStat = connect(this->ClientSocket, (sockaddr*)&server_info, sizeof(server_info));
    
    if (this->erStat != 0) {
        std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << "\n";
        closesocket(this->ClientSocket);
        WSACleanup();
        exit(1);
    }
    else 
        std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server\n";

    //Exchange text data between Server and Client. Disconnection if a Client send "xxx"

    std::vector <char> servBuff(BUFFER_SIZE), clientBuff(BUFFER_SIZE); // Buffers for sending and receiving data
    short packet_size = 0; // The size of sending / receiving packet in bytes
        
    while (true) {
        std::cout << "Your (Client) message to Server: ";
        fgets(clientBuff.data(), clientBuff.size(), stdin);

        // Check whether client like to stop chatting 
        if (clientBuff[0] == 'x' && clientBuff[1] == 'x' && clientBuff[2] == 'x') {
            shutdown(this->ClientSocket, SD_BOTH);
            closesocket(this->ClientSocket);
            std::cout << "You have closed connection\n";
            exit(0);
        }

        packet_size = send(this->ClientSocket, clientBuff.data(), clientBuff.size(), 0);

        if (packet_size == SOCKET_ERROR) {
            std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << "\n";
            closesocket(this->ClientSocket);
            WSACleanup();
            return;
        }

        packet_size = recv(this->ClientSocket, servBuff.data(), servBuff.size(), 0);

        if (packet_size == SOCKET_ERROR) {
            std::cout << "Can't receive message from Server. Error # " << WSAGetLastError() << "\n";
            closesocket(this->ClientSocket);
            WSACleanup();
            return;
        }
        else
            std::cout << "Server message: " << servBuff.data() << "\n";

    }
    closesocket(this->ClientSocket);
    WSACleanup();
}

int main() {
	WSADATA wsaData;
    int wsastart = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (wsastart != 0) {
        std::cerr << "WSAStartup failed: " << wsastart << "\n";
		WSACleanup();
        return wsastart;
    }
	std::cout << "WSAStartup was succesful\n";
    Client client;
    client.connect_to_server(6666, "127.0.0.1");
    std::cout << "Type # to stop";
    // std::cout << "Please, login";
    while (true) {
        std::cout << "What do you want?";
    }
}