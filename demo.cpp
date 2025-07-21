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

        handler.get("/", [](const string &query)
                    { 
                    string json = "{\"msg\":\"say no to UI - linh\"}";
                    return send_json(json, 200); });

        handler.get("/ls", [](const string &query){
            string dir = get_Param(query, "dir");
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

        handler.get("/test_redirect", [](const string &query)
                    { return redirect("/", 302); });

        handler.post("/echo", [](const string &body)
                     // handle x www
            {
            json json_body = json::parse(xwww_to_json(body));
            if(json_body.contains("name"))
            {
                string name = json_body["name"];
                json response_json = {{"echo", name}};
                return send_json(response_json.dump(), 200);
            }
            else
            {
                return send_json("{\"error\": \"Missing 'name' parameter\"}", 400);
            } });
        handler.post("/uploads",[](const string &body){
            json res = {{"msg", "File upload not implemented yet"}};
            return send_json(res.dump(), 501);
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
