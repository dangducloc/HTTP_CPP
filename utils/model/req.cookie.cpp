#include "../utils.h"
#include <iostream>
#include <string>

using namespace std;

string request_handler::set_cookie(string &response, Cookie cookie) {
    // add Cookie header to response
    string cookie_header = "Set-Cookie:" + cookie.toString();
    size_t header_end = response.find("\r\n\r\n");
    if (header_end == string::npos) return response; 

    // check if Set-Cookie already exists
    // If it exists, replace it; if not, insert before \r\n\r\n
    size_t pos = response.find("Set-Cookie:");
    if (pos == string::npos || pos > header_end) {
        response.insert(header_end, "\r\n" + cookie_header);
    } else {
        size_t end_pos = response.find("\r\n", pos);
        if (end_pos != string::npos) {
            response.replace(pos, end_pos - pos, cookie_header);
        } else {
            response.replace(pos, cookie_header.length(), cookie_header);
        }
    }
    return response;
}

string request_handler::get_cookie(const string &request, const string &name) {

    string cookie_string = get_header_value(request, "Cookie");
    if (cookie_string.empty()) {
        return "";
    }else{
        size_t start = cookie_string.find(name + "=");
        if (start == string::npos) {
            return "";
        }
        start += name.length() + 1; // Move past "name="
        size_t end = cookie_string.find(';', start);
        if (end == string::npos) {
            end = cookie_string.length();
        }
        return cookie_string.substr(start, end - start);
    }

}

