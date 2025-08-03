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


        server srv;
        srv.run(handler);
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
