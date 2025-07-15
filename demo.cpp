#include "./utils/utils.h"
#include "./lib/json.hpp"
#include <iostream>
#include <string>

using json = nlohmann::json;
using namespace std;

int main()
{
    try
    {
        request_handler handler;

        handler.get("/", [](const string &query)
        { 
                    string json = "{\"msg\":\"say no to UI - linh\"}";
                    return send_json(json, 200); 
        });

        handler.get("/echo", [](const string &query)
        {
            string name = get_Param(query, "name");
            if (name.empty()) name = "World";
            string json = "{\"echo\": \"" + name + "\"}";
            return send_json(json, 200); 
        });

        handler.get("/test_redirect", [](const string &query)
        { 
            return redirect("/", 302); 
        });

        handler.post("/echo", [](const string &body)
        //handle x www
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
            }
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
