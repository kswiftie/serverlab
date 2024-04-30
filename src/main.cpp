#include <iostream>
#include <future>
#include <thread>
#include "..\\include\\client.h"
#include "..\\include\\server.h"

// we need to link -lws2_32

int main() {
    Server myserver;
    // std::thread myThread(&Server::Server_listen, &myserver);

    // Запускаем поток
    // myThread.join();
    Client me;
    myserver.server_listen();
    me.connect_to_server();
    myserver.client_connect();
    // me.connect_to_server();
    return 0;
}