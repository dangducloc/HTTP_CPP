#include <vector>
#include "../utils.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <string_view>

using namespace std;

pair<string, string> request_handler::split_path_query(string_view rawpath) {
    size_t q = rawpath.find('?');
    if (q == string_view::npos) return {string(rawpath), ""};
    return {string(rawpath.substr(0, q)), string(rawpath.substr(q + 1))};
}

vector<char> request_handler::handle_route(
    const string& method,
    const string& path,
    const vector<char>& req
) {
    auto it_method = ROUTES.find(method);
    if (it_method != ROUTES.end()) {
        auto it_path = it_method->second.find(path);
        if (it_path != it_method->second.end())
            return it_path->second(req);
    }
    return serve_file(path);
}

vector<char> request_handler::serve_file(const string& path) {
    string filePath = WEB_ROOT + (path == "/" ? "/index.html" : path);
    string content = readFile(filePath);

    if (content.empty())
        return response("{\"error\": \"Not found\"}", 404);

    stringstream res;
    res << "HTTP/1.1 200 OK\r\n";
    res << "Server: " << getENV("SERVER_NAME") << "\r\n";
    res << "Content-Type: " << get_ContentType(path) << "\r\n";
    res << "Content-Length: " << content.size() << "\r\n";
    res << "Connection: close\r\n";
    res << "\r\n" << content;

    string result = res.str();
    return vector<char>(result.begin(), result.end());
}

vector<char> request_handler::POST_PUT_DEL(const vector<char> &req) {
    string_view req_view(req.data(), req.size());
    size_t method_end = req_view.find(' ');
    size_t path_end = req_view.find(' ', method_end + 1);

    string method(req_view.substr(0, method_end));
    string_view rawpath = req_view.substr(method_end + 1, path_end - method_end - 1);
    auto [path, query] = split_path_query(rawpath);

    return handle_route(method, path, req);
}

vector<char> request_handler::GET(const vector<char> &req) {
    string_view req_view(req.data(), req.size());
    size_t method_end = req_view.find(' ');
    size_t path_end = req_view.find(' ', method_end + 1);

    string_view rawpath = req_view.substr(method_end + 1, path_end - method_end - 1);
    auto [path, query] = split_path_query(rawpath);

    return handle_route("GET", path, req);
}

std::unordered_map<std::string, std::unordered_map<std::string, RouteHandler>> ROUTES;

void request_handler::get(const string &path, RouteHandler handler) {
    ROUTES["GET"][path] = handler;
}

void request_handler::post(const string &path, RouteHandler handler) {
    ROUTES["POST"][path] = handler;
}

void request_handler::put(const string &path, RouteHandler handler) {
    ROUTES["PUT"][path] = handler;
}

void request_handler::del(const string &path, RouteHandler handler) {
    ROUTES["DELETE"][path] = handler;
}


