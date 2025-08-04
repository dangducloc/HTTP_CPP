#include "../utils.h"
#include <string>
#include <vector>
#include "../../lib/json.hpp"
using namespace std;



string request_handler::get_cookie(const vector<char> &request, const string &name) {
    string req_str(request.begin(), request.end()); 
    string cookie_str = get_header_value(req_str, "Cookie");
    if (cookie_str.empty()) return "";

    istringstream ss(cookie_str);
    string token;

    while (getline(ss, token, ';')) {
        size_t eq = token.find('=');
        if (eq != string::npos) {
            string key = trim(token.substr(0, eq));
            string val = token.substr(eq + 1);
            if (key == name) return val;
        }
    }

    return "";
}
