#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "54000"
#define SERVER_IP "127.0.0.1"

void receive_messages(SOCKET clientSocket) {
    char buf[4096];
    while (true) {
        ZeroMemory(buf, 4096);
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived > 0) {
            std::cout << "Received: " << std::string(buf, 0, bytesReceived) << std::endl;
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

    getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &serverInfo);

    SOCKET clientSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    connect(clientSocket, serverInfo->ai_addr, (int)serverInfo->ai_addrlen);

    std::cout << "Connected to server." << std::endl;

    std::thread(receive_messages, clientSocket).detach();

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        send(clientSocket, msg.c_str(), msg.size(), 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
