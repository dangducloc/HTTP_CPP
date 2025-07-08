#include "../utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

using namespace std;

// Trim function
string trim(const string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

string getENV(const string& key) {
    ifstream envFile(".env");
    if (!envFile.is_open()) {
        cerr << "Error opening .env file" << endl;
        return "";
    }

    string line;
    while (getline(envFile, line)) {
        // cout << "[DEBUG] line: " << line << endl;

        size_t pos = line.find('=');
        if (pos != string::npos) {
            string k = trim(line.substr(0, pos));
            string v = trim(line.substr(pos + 1));
            if (k == key) {
                // cout << "[DEBUG] value: " << v << endl;
                envFile.close();
                return v;
            }
        }
    }

    envFile.close();
    return "";  // Return empty string if key not found
}
