#include "../utils.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <vector>
#include <regex>

using namespace std;


string request_handler::get_Param(const vector<char> &req, const string &key) {
    std::string_view req_sv(req.data(), req.size());

    // Find start of request line
    size_t first_line_end = req_sv.find("\r\n");
    if (first_line_end == std::string_view::npos)
        return "";

    std::string_view request_line = req_sv.substr(0, first_line_end);

    // Find query part in GET request
    size_t q_pos = request_line.find('?');
    std::string_view query;
    if (q_pos != std::string_view::npos) {
        size_t space_after = request_line.find(' ', q_pos);
        query = request_line.substr(q_pos + 1, space_after - q_pos - 1);
    } else {
        // If POST, find body after double CRLF
        std::string_view delimiter = "\r\n\r\n";
        size_t body_pos = req_sv.find(delimiter);
        if (body_pos != std::string_view::npos)
            query = req_sv.substr(body_pos + delimiter.size());
    }

    // Now parse key=value pairs
    size_t start = 0;
    while (start < query.size()) {
        size_t eq_pos = query.find('=', start);
        if (eq_pos == std::string_view::npos) break;
        std::string_view k = query.substr(start, eq_pos - start);
        size_t amp_pos = query.find('&', eq_pos);
        std::string_view v = query.substr(eq_pos + 1, (amp_pos == std::string_view::npos ? query.size() : amp_pos) - (eq_pos + 1));

        if (k == key) {
            return url_decode(std::string(v));
        }
        if (amp_pos == std::string_view::npos) break;
        start = amp_pos + 1;
    }

    return "";
}
