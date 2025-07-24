#include "../utils.h"
#include <iostream>
#include <sstream>

using namespace std;

unordered_map<string, RouteHandler> GET_ROUTES;
void request_handler::get(const string &path, RouteHandler handler)
{
    GET_ROUTES[path] = handler;
}

string get_Param(const string &query, const string &key)
{
    size_t start = query.find(key + "=");
    if (start == string::npos)
        return "";
    start += key.length() + 1;
    size_t end = query.find('&', start);
    return query.substr(start, (end == string::npos ? query.length() : end) - start);
}

string request_handler::handleGET(const string &req)
{   
    string method, rawpath, protocol;
    stringstream requestStream(req);
    requestStream >> method >> rawpath >> protocol;

    size_t qmark = rawpath.find('?');
    string path = (qmark != string::npos) ? rawpath.substr(0, qmark) : rawpath;
    string query = (qmark != string::npos) ? rawpath.substr(qmark + 1) : "";

    auto it = GET_ROUTES.find(path);
    if (it != GET_ROUTES.end()) {
        string response = it->second(query);
        return response;
    }

    string filePath = WEB_ROOT + (path == "/" ? "/index.html" : path);
    string content = readFile(filePath);

    if (content.empty()) {
        string error = send_json("{\"error\": \"Not found\"}", 404);
        return error;
    }

    stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "server: "<<getENV("SERVER_NAME")<<"\r\n";
    response << "Content-Type: " << get_ContentType(path) << "\r\n";
    response << "Content-Length: " << content.size() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;

    return response.str();
}
