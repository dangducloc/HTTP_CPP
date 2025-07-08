#include "../utils.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib") 

using namespace std;

server::server() {
    WSAData wsaData;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw runtime_error("WSAStartup failed: " + to_string(WSAGetLastError()));
    }

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        WSACleanup();
        throw runtime_error("Socket creation failed: " + to_string(WSAGetLastError()));
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        closesocket(server_fd);
        WSACleanup();
        throw runtime_error("Set socket options failed: " + to_string(WSAGetLastError()));
    }

    // Get PORT
    int PORT = stoi(getENV("PORT"));

    // Bind socket
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        closesocket(server_fd);
        WSACleanup();
        throw runtime_error("Bind failed: " + to_string(WSAGetLastError()));
    }

    // Listen
    if (listen(server_fd, 10) == SOCKET_ERROR) {
        closesocket(server_fd);
        WSACleanup();
        throw runtime_error("Listen failed: " + to_string(WSAGetLastError()));
    }

    cout << "[+] Server listening on port " << PORT << endl;
}


void server:: run(request_handler &handler) {
    while (true) {
        // Accept client
        SOCKET client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            cerr << "Accept failed: " << WSAGetLastError() << endl;
            continue;
        }

        // Read request
        char buffer[1024];
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the string
            string request(buffer);

            // Handle request
            string response = handler.handleRequest(request);

            // Send response
            send(client_socket, response.c_str(), response.length(), 0);
        }

        // Close client socket
        closesocket(client_socket);
    }

}