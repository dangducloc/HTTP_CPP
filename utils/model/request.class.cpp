#include "../utils.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

request_handler::request_handler()
{
    WEB_ROOT = getENV("WEB_ROOT");
    if (WEB_ROOT.empty())
    {
        WEB_ROOT = "./www"; // Default web root
    }
}


string request_handler::handleRequest(const string &request)
{
    stringstream requestStream(request);
    string method, path, protocol;
    requestStream >> method >> path >> protocol;

    // cout << "[DEBUG] Request: " << request << endl;

    string body = extract_body(request);

    string debug_line = "[DEBUG] Method: " + method + " | Path: " + path;

    string response;
    if (method == "GET") {
        response = handleGET(path);
    } else if (method == "POST") {
        debug_line += " | Body Length: " + to_string(body.length());
        debug_line += " | Body: " + body.substr(0, 50) + (body.length() > 50 ? "..." : "");
        response = handlePOST(path, body);
    } else if (method == "PUT" || method == "DELETE") {
        response = send_json("{\"error\": \"Method not allowed\"}", 405);
    } else {
        response = send_json("{\"error\": \"Unsupported method\"}", 501);
    }

    string statusLine = response.substr(0, response.find("\r\n"));
    debug_line = debug_line + " | Status: " + statusLine;
    cout << debug_line << endl;

    return response;
}
