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

string request_handler::handleGET(const string &rawPath)
{
    size_t qmark = rawPath.find('?');
    string path = (qmark != string::npos) ? rawPath.substr(0, qmark) : rawPath;
    string query = (qmark != string::npos) ? rawPath.substr(qmark + 1) : "";

    string filePath = WEB_ROOT + (path == "/" ? "/index.html" : path);
    string content = readFile(filePath);

    auto it = GET_ROUTES.find(path);
    if (it != GET_ROUTES.end())
    {
        return it->second(query); // gọi handler tương ứng
    }

    if (content.empty())
    {
        return send_json("{\"error\": \"File not found\"}", 404);
    }

    stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-Type: " << get_ContentType(path) << "\r\n";
    response << "Content-Length: " << content.size() << "\r\n";
    response << "Connection: close\r\n";
    response << "\r\n";
    response << content;

    return response.str();
}
