#include "../utils.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <cstdlib>

using namespace std;

server::server() {
    int PORT = stoi(getENV("PORT"));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        throw runtime_error("Socket creation failed");

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

void handle_client(socket_t client_socket, request_handler &handler) {
    vector<char> buffer;
    char temp[8192];
    ssize_t bytesReceived;

    string header_data;
    while (header_data.find("\r\n\r\n") == string::npos) {
        bytesReceived = recv(client_socket, temp, sizeof(temp), 0);
        if (bytesReceived <= 0) break;
        header_data.append(temp, temp + bytesReceived);
    }

    size_t content_length = 0;
    {
        size_t pos = header_data.find("Content-Length:");
        if (pos != string::npos) {
            pos += 15; // bỏ qua "Content-Length:"
            while (pos < header_data.size() && isspace(header_data[pos])) pos++;
            size_t end = header_data.find("\r\n", pos);
            content_length = std::stoul(header_data.substr(pos, end - pos));
        }
    }

    buffer.insert(buffer.end(), header_data.begin(), header_data.end());

    size_t header_end_pos = header_data.find("\r\n\r\n") + 4;
    size_t body_already = header_data.size() - header_end_pos;
    size_t body_needed = content_length > body_already ? content_length - body_already : 0;

    while (body_needed > 0) {
        bytesReceived = recv(client_socket, temp, sizeof(temp), 0);
        if (bytesReceived <= 0) break;
        buffer.insert(buffer.end(), temp, temp + bytesReceived);
        body_needed -= bytesReceived;
    }

    if (!buffer.empty()) {
        std::vector<char> response = handler.handleRequest(buffer);
        send(client_socket, response.data(), response.size(), 0);
    }

    close(client_socket);
}

void server::run(request_handler &handler) {
    ThreadPool pool(8); // 8 threads in the pool
    while (true) {
        socket_t client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pool.enqueue([client_socket, &handler]() {
            handle_client(client_socket, handler);
        });
    }
}
