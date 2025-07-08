#include "../utils.h"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

// Function to read file content
string readFile(const string& filePath) {
    ifstream file(filePath, ios::binary);
    if (!file.is_open()) {
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}