#include "../utils.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include "../../lib/json.hpp"

using json = nlohmann::json;
using namespace std;
namespace fs = std::filesystem;

string safe_name(string old_name){
    string new_name = old_name;
    for (char &c : new_name) {
        if (!isalnum((unsigned char)c) && c != '.' && c != '_' && c != '-') {
            c = '_';
        }
    }
    return new_name;
}


json request_handler::handle_file_upload(
    const vector<char> &req,
    const string &upload_dir,
    const string &required_field
) {
    try {
        fs::create_directories(upload_dir);
    } catch (const std::exception &e) {
        return {{"error", "Exception creating directory: " + string(e.what())}};
    }


    string_view req_sv(req.data(), req.size());
    size_t header_end = req_sv.find("\r\n\r\n");
    if (header_end == string_view::npos)
        return {{"error", "Invalid HTTP request"}};

    string_view header = req_sv.substr(0, header_end);
    string content_type = get_header_value(string(header), "Content-Type");

    string boundary = extract_boundary(content_type);
    if (boundary.empty()) {
        return {{"error", "Missing or invalid boundary"}};
    }

    vector<FormPart> parts = parse_multipart(req, boundary);
    json uploaded = json::array();

    for (const auto &part : parts) {
        if (part.name != required_field) continue;

        if (!part.filename.empty()) {
            string safe_filename = safe_name(part.filename);

            string file_path = upload_dir + "/" + safe_filename;
            ofstream out(file_path, ios::binary);
            if (!out) {
                return {{"error", "Failed to save file: " + safe_filename}};
            }

            out.write(reinterpret_cast<const char *>(part.data.data()), part.data.size());
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
