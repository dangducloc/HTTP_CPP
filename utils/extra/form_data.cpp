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
vector<FormPart> parse_multipart(const vector<char>& body, const string& boundary) {
    for (const auto& val : body) {
        std::cout << val ;
    }
    vector<FormPart> parts;
    string delimiter = "--" + boundary;
    string end_delimiter = delimiter + "--";

    auto pos = body.begin();
    
    while (true) {
        auto start = search(pos, body.end(), delimiter.begin(), delimiter.end());
        if (start == body.end()) break;
        start += delimiter.size();

        if (distance(start, body.end()) >= 2 && *(start) == '-' && *(start + 1) == '-') break;

        if (distance(start, body.end()) >= 2 && *(start) == '\r' && *(start + 1) == '\n') {
            start += 2;
        }

        auto next = search(start, body.end(), delimiter.begin(), delimiter.end());
        auto part_end = (next != body.end()) ? next : body.end();

        const string double_crlf = "\r\n\r\n";
        auto header_end = search(start, part_end, double_crlf.begin(), double_crlf.end());
        if (header_end == part_end) {
            pos = part_end;
            continue;
        }

        string headers(start, header_end);
        auto content_start = header_end + double_crlf.size();
        auto content_end = part_end;

        // Fix: Remove trailing \r or \n at the end of part content
        while (content_end != content_start && (*(content_end - 1) == '\r' || *(content_end - 1) == '\n')) {
            --content_end;
        }

        FormPart fp;
        istringstream header_stream(headers);
        string line;
        while (getline(header_stream, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();

            if (line.find("Content-Disposition:") != string::npos) {
                size_t name_pos = line.find("name=\"");
                if (name_pos != string::npos) {
                    size_t start = name_pos + 6;
                    size_t end = line.find("\"", start);
                    if (end != string::npos)
                        fp.name = line.substr(start, end - start);
                }

                size_t filename_pos = line.find("filename=\"");
                if (filename_pos != string::npos) {
                    size_t start = filename_pos + 10;
                    size_t end = line.find("\"", start);
                    if (end != string::npos)
                        fp.filename = line.substr(start, end - start);
                }
            }

            if (line.find("Content-Type:") != string::npos) {
                fp.content_type = trim(line.substr(13));
            }
        }

        fp.data = vector<unsigned char>(content_start, content_end);

        cout << "Parsed part: name=" << fp.name
             << ", filename=" << fp.filename
             << ", size=" << fp.data.size() << endl;

        parts.push_back(fp);

        pos = part_end;
    }

    return parts;
}
