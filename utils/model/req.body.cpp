#include "../../lib/json.hpp"
#include "../utils.h"
#include <string>
#include <iostream>

using namespace std;
using json = nlohmann::json;

json request_handler::body(const string &req) {
    string content_type = get_header_value(req, "Content-Type");
    string body = extract_body(req);

    if (content_type.find("application/json") != string::npos) {
        return json::parse(body);  // direct JSON parsing
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
                j[part.name] = part.data;
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
