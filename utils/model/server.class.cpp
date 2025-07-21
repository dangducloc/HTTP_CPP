#include "../utils.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>

using namespace std;

server::server() {
    int PORT = stoi(getENV("PORT"));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        throw runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(server_fd);
        throw runtime_error("Set socket options failed");
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        close(server_fd);
        throw runtime_error("Bind failed");
    }

    if (listen(server_fd, 10) < 0) {
        close(server_fd);
        throw runtime_error("Listen failed");
    }

    cout << "[+] Server listening on port " << PORT << endl;
}

void server::run(request_handler &handler) {
    while (true) {
        socket_t client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        char buffer[1024];
        ssize_t bytesReceived = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            string request(buffer);
            string response = handler.handleRequest(request);
            send(client_socket, response.c_str(), response.length(), 0);
        }

        close(client_socket);
    }
}
