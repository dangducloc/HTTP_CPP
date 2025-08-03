#include "../utils.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

request_handler::request_handler() {
    WEB_ROOT = getENV("WEB_ROOT");
    if (WEB_ROOT.empty())
    {
        WEB_ROOT = "./www"; // Default web root
    }
}

vector<char> request_handler::handleRequest(const vector<char> &raw_request)
{
    Logger logger;

    string request_text(raw_request.begin(), raw_request.end());
    size_t header_end = request_text.find("\r\n\r\n");
    if (header_end == string::npos) {
        return response("{\"error\": \"Invalid request format\"}", 400);
    }

    string headers = request_text.substr(0, header_end);
    vector<char> body(raw_request.begin() + header_end + 4, raw_request.end());

    string method, path, protocol;
    stringstream requestStream(headers);
    requestStream >> method >> path >> protocol;

    stringstream debug;
    debug << "[INFO] Method: " << method << " | Path: " << path;

    string content_type = get_header_value(headers, "Content-Type");
    if (!content_type.empty() && content_type.find("multipart/form-data") != string::npos) {
        string boundary = extract_boundary(content_type);
        debug << " | Boundary: " << boundary;
    }

    debug << " | Body Length: " << body.size();
    if (!body.empty()) {
        debug << " | Body Start: " << string(body.begin(), body.begin() + min((size_t)50, body.size()));
    }

    std::vector<char> res;
    if (method == "GET") {
        res = handleGET(raw_request);
    } else if (method == "POST") {
        res = handlePOST(raw_request);
    } else if (method == "PUT" || method == "DELETE") {
        res = response("{\"error\": \"Method not allowed\"}", 405);
    } else {
        res = response("{\"error\": \"Unsupported method\"}", 501);
    }

    // ✅ Convert response vector<char> to string for logging
    string status_line(res.begin(), res.end());
    size_t status_start = status_line.find(' ');
    size_t status_end = status_line.find("\r\n");
    string status = (status_start != string::npos && status_end != string::npos)
                        ? status_line.substr(status_start + 1, status_end - status_start - 1)
                        : "Unknown";

    debug << " | Status: " << status;
    cout << debug.str() << endl;
    logger.log(debug.str(), "request.log");

    return res;
}
