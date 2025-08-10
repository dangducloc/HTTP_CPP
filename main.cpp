#include "./utils/utils.h"
#include "./lib/json.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>
#include <string>

using json = nlohmann::json;
using namespace std;

//list all dir in WEB_ROOT
vector<char> list_directory(const string &dir) {
    vector<char> response_body;
    FILE *fp = popen(("ls -l " + dir).c_str(), "r");
    if (!fp) {
        throw runtime_error("Failed to run command");
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
        response_body.insert(response_body.end(), buffer, buffer + strlen(buffer));
    }
    pclose(fp);
    return response_body;
}

int main() {
    try {
        request_handler handler;

        handler.get("/", [](const vector<char> &req) -> vector<char> {
            this_thread::sleep_for(chrono::seconds(3));
            return response("{\"msg\":\"say no to UI - linh\"}", 200);
        });

        
        handler.post("/echo", [&](const vector<char> &req) -> vector<char> {
            json body = handler.body(req);

            if (body.empty()) {
                return response(R"({"error": "Empty body"})", 400);
            }

            if (!body.contains("msg")) {
                return response(R"({"error": "Missing 'msg' field"})", 400);
            }

            json res = {
                {"msg", body["msg"]},
                {"status", "success"}
            };
            return response(res.dump(), 200);
        });
        handler.post("/uploads", [&](const vector<char> &req) -> vector<char> {
            json upload_result = handler.handle_file_upload(req, getENV("WEB_ROOT")+ "/uploads", "file");
            if (upload_result.contains("error")) {
                return response(upload_result.dump(), 400);
            }
            return response(upload_result.dump(), 200);
        });
        handler.get("/ls",[&](const vector<char> &req) -> vector<char> {
            //list all dir in WEB_ROOT
            string web_root = getENV("WEB_ROOT");
            if (web_root.empty()) {
                return response(R"({"error": "WEB_ROOT environment variable not set"})", 500);
            }
            string dir = handler.get_Param(req, "dir");
            if (dir.empty()) {
                dir = web_root;
            } else {
                dir = web_root + "/" + dir;
            }
            vector<char> content = list_directory(dir);
            if (content.empty()) {
                return response(R"({"error": "Directory not found or empty"})", 404);
            }
            return response(string(content.begin(), content.end()), 200, "text/plain");

        });
        handler.get("/cookie", [&](const vector<char> &req) -> vector<char> {
            Cookie cookie("test", "coook", 3600, "/", "", false, true);
            string cookieHeader = "Set-Cookie: " + cookie.toString();
            return response("{\"msg\":\"Cookie set\"}",200, "application/json",{cookieHeader});
        });
        handler.get("/redirect", [](const vector<char> &req) -> vector<char> {
            return redirect("/", 302);
        });

        server srv;
        srv.run(handler);
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
