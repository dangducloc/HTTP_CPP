#include "../utils.h"
#include <string>
#include <vector>
#include "../../lib/json.hpp"
using namespace std;


string request_handler::set_cookie(string &response, const Cookie &cookie) {
    string cookie_header = "Set-Cookie: " + cookie.toString();

    size_t header_end = response.find("\r\n\r\n");
    if (header_end == string::npos) return response;

    response.insert(header_end, cookie_header + "\r\n");

    return response;
}

string request_handler::get_cookie(const string &request, const string &name) {
    string cookie_str = get_header_value(request, "Cookie");
    if (cookie_str.empty()) return "";

    size_t pos = cookie_str.find(name + "=");
    if (pos == string::npos) return "";

    pos += name.length() + 1;
    size_t end = cookie_str.find(';', pos);

    return cookie_str.substr(pos, end == string::npos ? string::npos : end - pos);
}
