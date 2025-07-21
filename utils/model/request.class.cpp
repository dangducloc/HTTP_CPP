#include "../utils.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

request_handler::request_handler() {
    WEB_ROOT = getENV("WEB_ROOT");
    if (WEB_ROOT.empty()) {
        WEB_ROOT = "./www"; // Default web root
    }
}

string request_handler::handleRequest(const string &request) {
    Logger logger;
    stringstream requestStream(request);
    string method, path, protocol;
    requestStream >> method >> path >> protocol;

    string body = extract_body(request);
    stringstream debug;
    debug << "[INFO] Method: " << method << " | Path: " << path;

    string response;

    if (method == "GET") {
        response = handleGET(path);
    } else if (method == "POST") {
        debug << " | Body Length: " << body.length();
        if (!body.empty()) {
            debug << " | Body: " << (body.length() > 100 ? body.substr(0, 100) + "..." : body);
        }
        response = handlePOST(path, body);
    } else if (method == "PUT" || method == "DELETE") {
        response = send_json("{\"error\": \"Method not allowed\"}", 405);
    } else {
        response = send_json("{\"error\": \"Unsupported method\"}", 501);
    }

    
    size_t status_start = response.find(' ');
    size_t status_end = response.find("\r\n");
    string status = (status_start != string::npos && status_end != string::npos) ?
                    response.substr(status_start + 1, status_end - status_start - 1) : "Unknown";

    debug << " | Status: " << status;
    cout << debug.str() << endl;
    logger.log(debug.str(), "request.log");

    return response;
}
