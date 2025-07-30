// yo wtf
#include "../utils.h"
#include <string>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;



// Helper: Get value of specific header
string get_header_value(const string& request, const string& header_name) {
    istringstream stream(request);
    string line;
    string prefix = header_name + ": ";

    while (getline(stream, line)) {
        if (line == "\r" || line.empty()) break;

        // Xóa \r cuối dòng nếu có
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.find(prefix) == 0) {
            return line.substr(prefix.length());
        }
    }
    return "";
}

// Helper: Extract boundary from Content-Type header
string extract_boundary(const string& content_type) {
    string prefix = "boundary=";
    size_t pos = content_type.find(prefix);
    if (pos == string::npos) return "";

    string boundary = content_type.substr(pos + prefix.length());

    // Remove quotes if present
    if (!boundary.empty() && boundary.front() == '"' && boundary.back() == '"') {
        boundary = boundary.substr(1, boundary.size() - 2);
    }

    return boundary;
}


// Parse multipart/form-data
vector<FormPart> parse_multipart(const string& body, const string& boundary) {
    vector<FormPart> parts;
    string delimiter = "--" + boundary;
    string end_delimiter = delimiter + "--";

    size_t pos = 0, next;

    while ((next = body.find(delimiter, pos)) != string::npos) {
        size_t part_start = next + delimiter.length();
        size_t part_end = body.find(delimiter, part_start);

        if (part_end == string::npos) break;

        string part = body.substr(part_start, part_end - part_start);
        pos = part_end;

        // Separate headers and content
        size_t header_end = part.find("\r\n\r\n");
        if (header_end == string::npos) continue;

        string headers = part.substr(0, header_end);
        string content = trim(part.substr(header_end + 4));

        FormPart fp;

        istringstream header_stream(headers);
        string line;
        while (getline(header_stream, line)) {
            if (line.find("Content-Disposition:") != string::npos) {
                size_t name_pos = line.find("name=\"");
                if (name_pos != string::npos) {
                    size_t start = name_pos + 6;
                    size_t end = line.find("\"", start);
                    fp.name = line.substr(start, end - start);
                }

                size_t filename_pos = line.find("filename=\"");
                if (filename_pos != string::npos) {
                    size_t start = filename_pos + 10;
                    size_t end = line.find("\"", start);
                    fp.filename = line.substr(start, end - start);
                }
            }

            if (line.find("Content-Type:") != string::npos) {
                fp.content_type = trim(line.substr(13));
            }
        }

        fp.data = content;
        parts.push_back(fp);
    }

    return parts;
}
