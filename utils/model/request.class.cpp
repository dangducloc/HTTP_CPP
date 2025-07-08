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

    string json_body;
    cout<< "[DEBUG] Request: " << request << endl;
    

    string debug_line = "[DEBUG] Method: " + method + " | Path: " + path;
    // if (method == "POST") {
    //     debug_line += " | Body: "+json_body+ " | Body Length: " + to_string(body.length());
    // }

    string response;
    if (method == "GET") {
        response = handleGET(path);
    } else if (method == "POST") {
        response = handlePOST(path, json_body);
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
