#include "index.h"
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

string redirect(const string &toPath, int code) {
    string status;
    if(code < 300 || code >= 400) {
        cout << "[ERROR] Invalid redirect code (Bro WTF!!!): " << code << endl;
        return "HTTP/1.1 400 Bro WTF!!!\r\n\r\n";
    }
    status = get_Status(code);

    stringstream res;
    res << "HTTP/1.1 " << status << "\r\n";
    res << "Location: " << toPath << "\r\n";
    res << "Content-Type: text/plain\r\n\r\n";
    res << "Redirecting to " << toPath;
    return res.str();
}

string send_json(string &jsonContent, int code ) {
    string status = get_Status(code);
    stringstream res;
    res << "HTTP/1.1 " << status << "\r\n";
    res << "Content-Type: application/json\r\n";
    res << "Content-Length: " << jsonContent.size() << "\r\n";
    res << "Connection: close\r\n\r\n";
    res << jsonContent;
    return res.str();
}

bool endsWith(const string &str, const string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

string get_ContentType(const string &path)
{
    if (endsWith(path, ".html"))
        return "text/html";
    if (endsWith(path, ".css"))
        return "text/css";
    if (endsWith(path, ".js"))
        return "application/javascript";
    if (endsWith(path, ".png"))
        return "image/png";
    if (endsWith(path, ".jpg") || endsWith(path, ".jpeg"))
        return "image/jpeg";
    if (endsWith(path, ".json"))
        return "application/json";
    return "text/plain";
}

string get_Status(int code)
{
    switch (code)
    {
    // ✅ 1xx Informational
    case 100: return "100 Continue";
    case 101: return "101 Switching Protocols";
    case 102: return "102 Processing";

    // ✅ 2xx Success
    case 200: return "200 OK";
    case 201: return "201 Created";
    case 202: return "202 Accepted";
    case 204: return "204 No Content";

    // ✅ 3xx Redirection
    case 301: return "301 Moved Permanently";
    case 302: return "302 Found";
    case 303: return "303 See Other";
    case 304: return "304 Not Modified";
    case 307: return "307 Temporary Redirect";
    case 308: return "308 Permanent Redirect";

    // ✅ 4xx Client Error
    case 400: return "400 Bad Request";
    case 401: return "401 Unauthorized";
    case 403: return "403 Forbidden";
    case 404: return "404 Not Found";
    case 405: return "405 Method Not Allowed";
    case 408: return "408 Request Timeout";
    case 429: return "429 Too Many Requests";

    // ✅ 5xx Server Error
    case 500: return "500 Internal Server Error";
    case 501: return "501 Not Implemented";
    case 502: return "502 Bad Gateway";
    case 503: return "503 Service Unavailable";
    case 504: return "504 Gateway Timeout";

    default:
        return to_string(code) + " Unknown Status";
    }
}
