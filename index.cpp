#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib") // Link Winsock library

const int PORT = 8080;
const std::string WEB_ROOT = "./www"; // Directory for static files

// Function to read file content
std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Function to handle HTTP request
std::string handleRequest(const std::string& request) {
    std::stringstream requestStream(request);
    std::string method, path, protocol;
    requestStream >> method >> path >> protocol;

    if (method != "GET") {
        return "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
    }

    std::string filePath = WEB_ROOT + (path == "/" ? "/utils.html" : path);
    std::string content = readFile(filePath);

    if (content.empty()) {
        return "HTTP/1.1 404 Not Found\r\n\r\n<h1>404 Not Found</h1>";
    }

    std::stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << content.length() << "\r\n";
    response << "\r\n";
    response << content;

    return response.str();
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << "\n";
        return 1;
    }

    // Create socket
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // Set socket options
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    // Bind socket
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << "\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Listen
    if (listen(server_fd, 10) == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "Server running on port " << PORT << "\n";

    while (true) {
        // Accept client
        SOCKET client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        // Read request
        char buffer[1024] = {0};
        recv(client_socket, buffer, 1024, 0);
        std::string request(buffer);

        // Handle request and send response
        std::string response = handleRequest(request);
        send(client_socket, response.c_str(), response.length(), 0);

        // Close client socket
        closesocket(client_socket);
    }

    // Cleanup
    closesocket(server_fd);
    WSACleanup();
    return 0;
}