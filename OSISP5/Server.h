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


