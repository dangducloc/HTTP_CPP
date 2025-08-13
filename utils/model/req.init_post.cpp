#include "../utils.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <vector>
#include <regex>

using namespace std;

unordered_map<string, RouteHandler> POST_ROUTES;

void request_handler::post(const string &path, RouteHandler handler) {
    POST_ROUTES[path] = handler;
}

string xwww_to_json(const string &body) {
    istringstream ss(body);
    string key_value;

    unordered_map<string, vector<string>> array_data;
    unordered_map<string, string> normal_data;

    regex array_brackets(R"((\w+)\[\])");
    regex indexed_array(R"((\w+)\[\d+\])");

    while (getline(ss, key_value, '&')) {
        size_t eq_pos = key_value.find('=');
        if (eq_pos == string::npos) continue;

        string raw_key = key_value.substr(0, eq_pos);
        string raw_value = key_value.substr(eq_pos + 1);

        string key = url_decode(raw_key);
        string value = url_decode(raw_value);

        smatch match;
        if (regex_match(key, match, array_brackets) || regex_match(key, match, indexed_array)) {
            string array_key = match[1];
            array_data[array_key].push_back(value);
        } else {
            normal_data[key] = value;
        }
    }

    stringstream json;
    json << "{";
    bool first = true;

    for (const auto &[key, value] : normal_data) {
        if (!first) json << ",";
        json << "\"" << key << "\":\"" << value << "\"";
        first = false;
    }

    for (const auto &[key, vec] : array_data) {
        if (!first) json << ",";
        json << "\"" << key << "\":[";
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) json << ",";
            json << "\"" << vec[i] << "\"";
        }
        json << "]";
        first = false;
    }

    json << "}";
    return json.str();
}

vector<char> request_handler::handlePOST(const vector<char> &req) {
    string req_str(req.begin(), req.end());
    stringstream requestStream(req_str);
    string method, rawpath, protocol;
    requestStream >> method >> rawpath >> protocol;

    size_t qmark = rawpath.find('?');
    string path = (qmark != string::npos) ? rawpath.substr(0, qmark) : rawpath;

    auto it = POST_ROUTES.find(path);
    if (it != POST_ROUTES.end()) {
        return it->second(req);
    } else {
        return response("{\"error\": \"Route not found\"}", 404);
    }
}
