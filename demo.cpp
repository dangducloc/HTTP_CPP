#include "./utils/index.h"
#include <iostream>
#include <string>
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
