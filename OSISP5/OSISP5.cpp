#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <map>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "54000"

std::map<SOCKET, std::string> clients;

void handle_client(SOCKET clientSocket) {
    char buf[4096];
    while (true) {
        ZeroMemory(buf, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived <= 0) {
            // Клиент отключился
            closesocket(clientSocket);
            clients.erase(clientSocket);
            break;
        }

        std::string msg(buf, 0, bytesReceived);
        // Пересылка сообщения всем клиентам
        for (const auto& client : clients) {
            if (client.first != clientSocket) {
                send(client.first, buf, bytesReceived, 0);
            }
        }
    }
}


int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    struct addrinfo hints, * serverInfo;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, DEFAULT_PORT, &hints, &serverInfo);

    SOCKET listenSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    bind(listenSocket, serverInfo->ai_addr, (int)serverInfo->ai_addrlen);
    listen(listenSocket, SOMAXCONN);

    std::cout << "Server is listening on port " << DEFAULT_PORT << std::endl;

    while (true) {
        SOCKET clientSocket = accept(listenSocket, NULL, NULL);
        clients[clientSocket] = "Client" + std::to_string(clients.size() + 1);
        std::thread(handle_client, clientSocket).detach();
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}

