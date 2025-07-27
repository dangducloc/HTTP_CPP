#include <fstream>
#include <string>
#include <vector>
#include "../../lib/json.hpp"
#include "../utils.h"

using json = nlohmann::json;
using namespace std;

json request_handler::handle_file_upload(const string &request, const string &upload_dir, const string &required_field) {
    string content_type = get_header_value(request, "Content-Type");
    string boundary = extract_boundary(content_type);

    if (boundary.empty()) {
        return {{"error", "Missing or invalid boundary"}};
    }

    string body = extract_body(request);
    vector<FormPart> parts = parse_multipart(body, boundary);

    json uploaded = json::array();

    for (const auto &part : parts) {
        // ✅ Check if the part matches the expected field name
        if (part.name != required_field) {
            continue;  // skip unrelated fields
        }

        if (!part.filename.empty()) {
            // Optionally sanitize filename
            string safe_filename = part.filename;
            for (char &c : safe_filename) {
                if (c == '/' || c == '\\') c = '_';
            }

            // Save the file
            ofstream out(upload_dir + "/" + safe_filename, ios::binary);
            if (!out) {
                return {{"error", "Failed to save file: " + safe_filename}};
            }

            out << part.data;
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
