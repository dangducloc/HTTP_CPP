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

string exec_command(const string &cmd)
{
    array<char, 128> buffer;
    string result;

    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw runtime_error("popen() failed");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    return result;
}

int main()
{
    try
    {
        request_handler handler;

        handler.get("/", [](const string &req)
                { 
                // thread test
                this_thread::sleep_for(chrono::seconds(3));
                string json = "{\"msg\":\"say no to UI - linh\"}";
                return send_json(json, 200); });

        handler.get("/ls", [&](const string &req){
            string dir = handler.get_Param(req, "dir");
            if (dir.empty()) dir = "/";

            // Basic injection prevention
            if (dir.find(';') != string::npos || dir.find('&') != string::npos) {
                json err = {{"error", "Invalid path"}};
                return send_json(err.dump(), 400);
            }

            string command = "ls \"" + dir + "\"";
            string output;

            try {
                output = exec_command(command);
            } catch (const exception& e) {
                json err = {{"error", e.what()}};
                return send_json(err.dump(), 500);
            }

            istringstream ss(output);
            string line;
            json arr = json::array();
            while (getline(ss, line)) {
                if (!line.empty()){
                    json ele = {{"dir", line}};
                    
                    arr.push_back(ele);
                    
                }
            }
            json result = {{"result", arr}};

            return send_json(result.dump(), 200); });

        handler.get("/test_redirect", [](const string &req)
                    { return redirect("/", 302); });

        handler.post("/echo", [&](const string &req)
            {
                json body = handler.body(req);
                if (body.empty())
                {
                    return send_json("{\"error\": \"Empty body\"}", 400);
                }
                if(!body.contains("msg"))
                {
                    return send_json("{\"error\": \"Missing 'msg' field\"}", 400);
                }
                json response = {
                    {"msg", body["msg"]},
                    {"status", "success"}
                };
                return send_json(response.dump(), 200);
            });
        handler.post("/uploads",[&](const string &req)
            {
                json result = handler.handle_file_upload(req,getENV("WEB_ROOT")+"/uploads","file");
                int status = result.contains("error") ? 400 : 200;
                return send_json(result.dump(), status);
            });
        server srv;
        srv.run(handler);
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
