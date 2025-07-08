#ifndef utils
#define utils

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <unordered_map>
#include <functional>

std::string getENV(const std::string &key);
std::string get_Param(const std::string &query, const std::string &key);
std::string readFile(const std::string &filePath);
std::string redirect(const std::string &toPath, int code);
std::string get_ContentType(const std::string &path);
std::string get_Status(int code);
std::string send_json(std::string jsonContent, int code );
// Routing support
using RouteHandler = std::function<std::string(const std::string &query)>;
extern std::unordered_map<std::string, RouteHandler> GET_ROUTES;
extern std::unordered_map<std::string, RouteHandler> POST_ROUTES;


class request_handler
{
public:
    std::string WEB_ROOT;
    int PORT;

    request_handler();
    std::string handleRequest(const std::string &request);
    std::string handleGET(const std::string &path);
    void get(const std::string &path, RouteHandler handler);
    std::string handlePOST(const std::string &path, const std::string &body);
    void post(const std::string &path, RouteHandler handler);
};

class server
{
public:
    WSAData wsaData;
    SOCKET server_fd;

    server();
    void run(request_handler &handler);
};

#endif // utils
