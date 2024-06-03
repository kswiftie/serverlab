#include "..\\include\\server.h"
#include "database.cpp"

database musicdb;

Server::Server(std::string server_ip, uint16_t port_value) {
	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	this->erStat = inet_pton(AF_INET, server_ip.c_str(), &ip_to_num);
	
	if (this->erStat <= 0) {
		std::cout << "Error in IP translation to special numeric format\n";
		exit(1);
	}
    
	// Server socket initialization
	this->ServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (this->ServSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << "\n";
		closesocket(this->ServSock);
		WSACleanup();
		exit(1);
	}

	// Server socket binding
	sockaddr_in servInfo;
	ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure
				
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(port_value);

	this->erStat = bind(this->ServSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (this->erStat != 0) {
		std::cout << "Error Socket binding to server info. Error # " << WSAGetLastError() << "\n";
		stop();
		exit(1);
	}
	this->status = true;
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
		this->erStat = listen(this->ServSock, SOMAXCONN);
		if (this->erStat != 0) {
			std::cout << "Can't start to listen to. Error # " << WSAGetLastError() << "\n";
			stop();
			continue;
		}

		sockaddr_in clientInfo;
		ZeroMemory(&clientInfo, sizeof(clientInfo));
		int clientInfo_size = sizeof(clientInfo);
		SOCKET clientSocket = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);

		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Accept error" << WSAGetLastError() << "\n";
			continue;
		}
		response(clientSocket, clientInfo);
	}
}

void Server::response(SOCKET client, sockaddr_in clientInfo) {
	char userip[50];
	inet_ntop(AF_INET, &clientInfo.sin_addr, userip, INET_ADDRSTRLEN); // Convert connected client's IP to standard string format
	char buffer[BUFF_SIZE];
	int request = recv(client, buffer, BUFF_SIZE, 0);
	if (request != SOCKET_ERROR) {
		/*
		std::string response_header = "\
HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=UTF-8\r\n\
Content-Length: 15\r\n\r\n";
		*/
		std::string response_header = ""; // remove

		std::string response_content = get_response(buffer, userip);

		send(client, (response_header + response_content).c_str(), (response_header + response_content).size(), 0);
	}
	closesocket(client);
}

std::string Server::get_response(std::string request, std::string userip) {
	std::string res = "Incorrect request\n";
    if (request == help) {
        res =gethelp();
    }
    else if (std::regex_match(request, login)) {
		std::string name, password;
		int name_id = request.find("username=") + 9, password_id = request.find("password=") + 9;
		for (int i = name_id; i < password_id - 10; ++i) name += request[i];
		for (int i = password_id; i < request.size(); ++i) password += request[i];
        res = login_user(name, password, userip);
    }
    else if (request == masklogout) {
        res = logout(userip);
    }
    else if (std::regex_match(request, reg)) {
        std::string name, password;
		int name_id = request.find("username=") + 9, password_id = request.find("password=") + 9;
		for (int i = name_id; i < password_id - 10; ++i) name += request[i];
		for (int i = password_id; i < request.size(); ++i) password += request[i];
        res = reg_user(name, password, userip);
    }
	else if (std::regex_match(request, listen_song_by_info)) {
		auto it = this->loggedin_users.find(userip);
		if (it != this->loggedin_users.end()) {
			int song_id = request.find("song=") + 5,
			album_id = request.find("&album=") + 7,
			artist_id = request.find("&artist=") + 8;
			std::string song = "", album = "", artist = "";
			for (int i = song_id; i < album_id - 7; ++i)
				song += request[i];
			for (int i = album_id; i < artist_id - 8; ++i)
				album += request[i];
			for (int i = artist_id; i < request.size(); ++i)
				artist += request[i];
        	res = listen_song(song, album, artist, userip);
		}
		else
			res = "You have to login before\n";
        
	}
	else if (std::regex_match(request, listen_album_by_info)) {
		auto it = this->loggedin_users.find(userip);
		if (it != this->loggedin_users.end()) {
			int album_id = request.find("album=") + 6,
			artist_id = request.find("&artist=") + 8;
			std::string album = "", artist = "";
			for (int i = album_id; i < artist_id - 8; ++i)
				album += request[i];
			for (int i = artist_id; i < request.size(); ++i)
				artist += request[i];
        	res = listen_album(album, artist, userip);
		}
		else
			res = "You have to login before\n";
	}
    else if (request == "GET /HTTP/127.0.0.1:8080") {
        res = "Incorrect request. Enter help for more information\n";
    }
    return res;
}

std::string Server::reg_user(std::string name, std::string password, std::string userip) {
	std::string columns = "(name, password)";
	musicdb.get_info_from_table("client", columns);
	std::ifstream file("tmp.txt");
	if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
			if (line.find(name) != std::string::npos) {
				file.close();
				return "User with that name has already been registered. Try a different username\n";
			}
        }
        file.close();
    }
	std::hash<std::string> hasher;
    size_t hashpassword = hasher(password);
	std::string str_hash_password = std::to_string(hashpassword);
	name = (std::string)"'" + name + (std::string)"'";
	password = (std::string)"'" + password + (std::string)"'";
	std::string res = (std::string)"(" + name + (std::string)", " + str_hash_password + (std::string)")";
    musicdb.add_entry("client", columns, res);
	return "You succesful registered\n";
}

std::string Server::gethelp() {
	return "list of availavle commands:\n\
help\n\
login\n\
logout\n\
register\n\
listen song (enter style: song name, album name, artist name.)\n\
listen album (enter style: album name, artist name.)\n";
}

std::string Server::login_user(std::string name, std::string password, std::string userip) {
	std::hash<std::string> hasher;
	std::string columns = "(name, ' ', password)";
    size_t hashpassword = hasher(password);
	musicdb.get_info_from_table("client", columns);
	std::string str_hash_password = std::to_string(hashpassword);
	std::ifstream file("tmp.txt");
	if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
			if (line.find(name) != std::string::npos && line.find(str_hash_password) != std::string::npos) {
				this->loggedin_users[userip] = name;
				file.close();
				return "You succesful logged in\n";
			}
        }
        file.close();
    }
	return "Username or password is incorrect\n";
}

std::string Server::logout(std::string userip) {
	auto it = this->loggedin_users.find(userip);
	if (it != this->loggedin_users.end()) {
		this->loggedin_users.erase(userip);
		return "You succesful logged out\n";
	}
	return "Youre not logged in\n";
}

std::string Server::listen_song(std::string song,
std::string album,
std::string artist, std::string clientip) {
	std::string columns = "(id, ' ', name)";
	musicdb.get_info_from_table("song", columns);
	std::vector <std::string> songs_id = get_id(song);
	if (song == "" && album == "" && artist == "")
		return "Incorrect data";
	std::string request = "SELECT song.id FROM song \
INNER JOIN album ON album.id IN (SELECT id FROM album WHERE album.name = '" + album + "') \
INNER JOIN artist ON artist.id IN  (SELECT id FROM artist WHERE artist.name = '" + artist + "') \
INNER JOIN album_song ON album_song.album_id = album.id AND album_song.song_id = song.id \
INNER JOIN artist_song ON artist_song.artist_id = artist.id AND artist_song.song_id = song.id;";
	musicdb.hard_request(request);
	std::ifstream file("tmp.txt");
	std::vector <std::string> res;
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			for (std::string s : songs_id) {
				if (line.find(s) != std::string::npos) {
					update_song_info(s, this->loggedin_users[clientip]);
					file.close();
					return ("You listened song " + song + " by " + artist);
				}
			}
		}
	}
	file.close();
	return "Song was not found\n";
}

std::vector <std::string> Server::get_id(std::string name) {
	std::ifstream file("tmp.txt");
	std::vector <std::string> res;
	if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
			if (line.find(name) != std::string::npos) {
				std::istringstream splitline(line);
				std::string id;
				splitline >> id;
				res.push_back(id);
			}
        }
        file.close();
    }
	return res;
}

std::string Server::listen_album(std::string album, std::string artist, std::string clientip) {
	std::string request = "SELECT CONCAT(song.id, ' songname') FROM song \
INNER JOIN album ON album.id IN (SELECT id FROM album WHERE album.name = '" + album + "') \
INNER JOIN artist ON artist.id IN (SELECT id FROM artist WHERE artist.name = '" + artist + "') \
INNER JOIN album_song ON album_song.album_id = album.id AND album_song.song_id = song.id \
INNER JOIN artist_song ON artist_song.artist_id = artist.id AND artist_song.song_id = song.id;";
	musicdb.hard_request(request);
	std::vector <std::string> songs_id = get_id("songname");
	int n = songs_id.size();
	if (n == 0) return "Album was not found\n";
	for (int i = 0; i < n; ++i) {
		update_song_info(songs_id[i], this->loggedin_users[clientip]);
	}
	return ("You listened album " + album + " by " + artist);
}

void Server::update_song_info(std::string song_id, std::string client) {
	std::string columns = "(id, ' ', name)";
	musicdb.get_info_from_table("client", columns);
	std::vector <std::string> tmp = get_id(client);
	std::string client_id = tmp[0];
	std::string request = "INSERT INTO song_listenings \
(song_id, client_id, listenings_count) \
VALUES (" + song_id + ", " + client_id + ", 1);";
	
	columns = "(song_id, ' ', client_id)";
	musicdb.get_info_from_table("song_listenings", columns);

	std::ifstream file("tmp.txt");
	if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
			if (line.find(song_id) != std::string::npos && line.find(client_id) != std::string::npos) {
				request = "UPDATE song_listenings \
SET listenings_count = listenings_count + 1 \
WHERE (song_id = " + song_id + ") \
AND (client_id = " + client_id + ");";
				break;
			}
        }
    }
	file.close();
	musicdb.hard_request(request);
}

int main() {
	system("chcp 1251 >NUL 2>&1");
	WSADATA wsaData;
    int wsastart = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsastart != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << "\n";
		WSACleanup();
        return wsastart;
    }
	short port = 8080;
	std::string host = "127.0.0.1", str_port = std::to_string(port);

	Server server(host, port);
	short n = std::thread::hardware_concurrency();
	std::vector <std::thread> threads(n);
	for (int i = 0; i < n; ++i)
		threads.emplace_back((std::thread)(&Server::waitingAcceptLoop, server));
	for (auto& thread : threads)
		thread.join();
}