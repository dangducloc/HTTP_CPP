#ifndef utils
#define utils

#include <string>
#include <unordered_map>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Define socket type alias for consistency
typedef int socket_t;

// Helper functions
std::string readFile(const std::string &filePath);
std::string getENV(const std::string &key);

std::string get_Param(const std::string &query, const std::string &key);
std::string redirect(const std::string &toPath, int code);
std::string get_ContentType(const std::string &path);
std::string get_Status(int code);
std::string send_json(std::string jsonContent, int code);

std::string extract_body(const std::string &request);
std::string url_decode(const std::string &value);
std::string xwww_to_json(const std::string &body);

// Routing support
using RouteHandler = std::function<std::string(const std::string &query)>;
extern std::unordered_map<std::string, RouteHandler> GET_ROUTES;
extern std::unordered_map<std::string, RouteHandler> POST_ROUTES;

// Request handler class
class request_handler
{
public:
    std::string WEB_ROOT;
    request_handler();
    std::string handleRequest(const std::string &request);
    std::string handleGET(const std::string &path);
    void get(const std::string &path, RouteHandler handler);
    std::string handlePOST(const std::string &path, const std::string &body);
    void post(const std::string &path, RouteHandler handler);
};

// Server class
class server
{
public:
    socket_t server_fd;

    server();
    void run(request_handler &handler);
};

class Logger
{
private:
    std::string logDir;
    std::string getCurrentTime();
    bool logDirExists();
    void createLogDir();

public:
    Logger();
    void log(const std::string &message, const std::string &filename);
    std::string getLogDir() const;


};

#endif // utils
