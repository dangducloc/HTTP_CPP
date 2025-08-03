#include "../utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "../../lib/json.hpp"

using json = nlohmann::json;
using namespace std;

json request_handler::handle_file_upload(const vector<char> &req, const string &upload_dir, const string &required_field) {
    string request(req.begin(), req.end());
    string content_type = get_header_value(request, "Content-Type");
    string boundary = extract_boundary(content_type);

    if (boundary.empty()) {
        return {{"error", "Missing or invalid boundary"}};
    }

    string body = extract_body(request);
    vector<FormPart> parts = parse_multipart(body, boundary);

    json uploaded = json::array();

    for (const auto &part : parts) {
        if (part.name != required_field) {
            continue;  // skip unrelated fields
        }

        if (!part.filename.empty()) {
            string safe_filename = part.filename;
            for (char &c : safe_filename) {
                if (c == '/' || c == '\\') c = '_';
            }

            ofstream out(upload_dir + "/" + safe_filename, ios::binary);
            if (!out) {
                return {{"error", "Failed to save file: " + safe_filename}};
            }

            out.write(reinterpret_cast<const char *>(part.data.data()), part.data.size());
            out.close();

            uploaded.push_back({
                {"field", part.name},
                {"filename", safe_filename},
                {"size", part.data.size()},
                {"content_type", part.content_type}
            });
        }
    }

    if (uploaded.empty()) {
        return {{"error", "Missing file with field name: " + required_field}};
    }

    return {{"uploaded", uploaded}};
}
