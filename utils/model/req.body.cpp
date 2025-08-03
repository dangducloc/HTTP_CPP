#include "../../lib/json.hpp"
#include "../utils.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using json = nlohmann::json;

json request_handler::body(const vector<char> &req) {
    string request(req.begin(), req.end());
    string content_type = get_header_value(request, "Content-Type");
    string body = extract_body(request);

    if (content_type.find("application/json") != string::npos) {
        return json::parse(body);
    }

    if (content_type.find("application/x-www-form-urlencoded") != string::npos) {
        return json::parse(xwww_to_json(body));
    }

    if (content_type.find("multipart/form-data") != string::npos) {
        string boundary = extract_boundary(content_type);
        if (boundary.empty()) {
            throw runtime_error("Missing boundary in Content-Type");
        }

        vector<FormPart> parts = parse_multipart(body, boundary);
        json j;
        for (const auto &part : parts) {
            if (part.filename.empty()) {
                j[part.name] = string(part.data.begin(), part.data.end());
            } else {
                j[part.name] = {
                    {"filename", part.filename},
                    {"content_type", part.content_type},
                    {"size", part.data.size()}
                };
            }
        }
        return j;
    }

    throw runtime_error("Unsupported Content-Type: " + content_type);
}
