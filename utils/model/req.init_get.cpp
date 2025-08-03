#include "../utils.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <vector>
#include <regex>

using namespace std;

unordered_map<string, RouteHandler> GET_ROUTES;

void request_handler::get(const string &path, RouteHandler handler) {
    GET_ROUTES[path] = handler;
}

string request_handler::get_Param(const string &query, const string &key) {
    size_t start = query.find(key + "=");
    if (start == string::npos)
        return "";
    start += key.length() + 1;
    size_t end = query.find('&', start);
    return query.substr(start, (end == string::npos ? query.length() : end) - start);
}

vector<char> request_handler::handleGET(const vector<char> &req) {
    string req_str(req.begin(), req.end());
    string method, rawpath, protocol;
    stringstream requestStream(req_str);
    requestStream >> method >> rawpath >> protocol;

    size_t qmark = rawpath.find('?');
    string path = (qmark != string::npos) ? rawpath.substr(0, qmark) : rawpath;
    string query = (qmark != string::npos) ? rawpath.substr(qmark + 1) : "";

    auto it = GET_ROUTES.find(path);
    if (it != GET_ROUTES.end()) {
        return it->second(req);
    }

    string filePath = WEB_ROOT + (path == "/" ? "/index.html" : path);
    string content = readFile(filePath);

    if (content.empty()) {
        return response("{\"error\": \"Not found\"}", 404);
    }

    stringstream res;
    res << "HTTP/1.1 200 OK\r\n";
    res << "server: " << getENV("SERVER_NAME") << "\r\n";
    res << "Content-Type: " << get_ContentType(path) << "\r\n";
    res << "Content-Length: " << content.size() << "\r\n";
    res << "Connection: close\r\n";
    res << "\r\n";
    res << content;

    string result_str = res.str();
    return vector<char>(result_str.begin(), result_str.end());
}
