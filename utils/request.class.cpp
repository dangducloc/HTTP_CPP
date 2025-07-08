#include "index.h"
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

    cout << "[DEBUG] Method: " << method << " | Path: " << path ;

    string response;
    if (method == "GET")
    {
        response = handleGET(path);
    }
    else
    {
        response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
    }

    string statusLine = response.substr(0, response.find("\r\n"));
    cout << " | Status: " << statusLine << endl;

    return response;
}
