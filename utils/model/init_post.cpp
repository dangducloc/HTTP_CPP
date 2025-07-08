#include "../utils.h"
#include <iostream>
#include <sstream>

using namespace std;

unordered_map<string, RouteHandler> POST_ROUTES;

void request_handler::post(const string &path, RouteHandler handler) {
    POST_ROUTES[path] = handler;
}

string request_handler::handlePOST(const string &path, const string &body) {
    auto it = POST_ROUTES.find(path);
    if (it != POST_ROUTES.end()) {
        return it->second(body);
    } else {
        return "HTTP/1.1 404 Not Found\r\n\r\n";
    }
}

string get_json(const string &request) {
// [DEBUG] Request: POST /echo HTTP/1.1
// Content-Type: application/json
// User-Agent: PostmanRuntime/7.39.1
// Accept: */*
// Cache-Control: no-cache
// Postman-Token: 6e634342-e4c1-4d32-80b3-e0513ba2fcd2
// Host: localhost:8080
// Accept-Encoding: gzip, deflate, br
// Connection: keep-alive
// Content-Length: 14

// {"msg":"lmao"}


    string json_body;
    // This function is a placeholder for extracting JSON from the body.
    // In a real implementation, you would parse the body to extract the JSON content.
    return json_body; // Assuming body is already in JSON format
}

string get_x_www_form_urlencoded(const string &request) {

// [DEBUG] Request: POST /echo HTTP/1.1
// User-Agent: PostmanRuntime/7.39.1
// Accept: */*
// Cache-Control: no-cache
// Postman-Token: 123207f7-a896-4679-9170-9d14ed378551
// Host: localhost:8080
// Accept-Encoding: gzip, deflate, br
// Connection: keep-alive
// Content-Type: application/x-www-form-urlencoded
// Content-Length: 15

// msg=lmao&msg1=1

    string data;
    // This function is a placeholder for extracting x-www-form-urlencoded data from the body.
    // In a real implementation, you would parse the body to extract the key-value pairs.
    return data; // Assuming body is empty or not in x-www-form-urlencoded format
}

string get_form_data(const string &request) {
// [DEBUG] Request: POST /echo HTTP/1.1
// User-Agent: PostmanRuntime/7.39.1
// Accept: */*
// Cache-Control: no-cache
// Postman-Token: 15cf1f8c-5ca6-4a53-ae12-725cd5cf9b39
// Host: localhost:8080
// Accept-Encoding: gzip, deflate, br
// Connection: keep-alive
// Content-Type: multipart/form-data; boundary=--------------------------754775720362993245836684
// Content-Length: 370

// ----------------------------754775720362993245836684
// Content-Disposition: form-data; name="msg"

//  lmao
// ----------------------------754775720362993245836684
// Content-Disposition: form-data; name="msg1"

// 2
// ----------------------------754775720362993245836684
// Content-Disposition: form-data; name="msg"

// 4
// ----------------------------754775720362993245836684--
    string data;
    // This function is a placeholder for extracting form-data from the body.
    // In a real implementation, you would parse the body to extract the key-value pairs.
    return data; // Assuming body is empty or not in multipart/form-data format
}





