#ifndef utils
#define utils

#include <string>
#include "../lib/json.hpp"
#include <unordered_map>
#include <functional>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include "./thread_pool/thread_pool.hpp"
using json = nlohmann::json;
// Define socket type alias for consistency
typedef int socket_t;

// Helper functions
std::string readFile(const std::string &filePath);
std::string getENV(const std::string &key);

std::string trim(const std::string &str);


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

struct FormPart {
   std::string name;
   std::string filename;
   std::string content_type;
   std::string data;
};


// Request handler class
class request_handler
{
public:
    std::string WEB_ROOT;
    request_handler();
    std::string handleRequest(const std::string &request);
    std::string handleGET(const std::string &path);
    void get(const std::string &path, RouteHandler handler);
    std::string handlePOST(const std::string &body);
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
std::string get_header_value(const std::string& request, const std::string& header_name);
std::string extract_boundary(const std::string& content_type);
std::vector<FormPart> parse_multipart(const std::string& body, const std::string& boundary);
json handle_file_upload(const std::string &request, const std::string &upload_dir, const std::string &required_field);


#endif // utils
