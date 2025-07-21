#include "../utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <ctime>
#include <cerrno>

using namespace std;

string Logger::getCurrentTime()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buffer);
}

bool Logger::logDirExists()
{
    struct stat info;
    return (stat(logDir.c_str(), &info) == 0 && (info.st_mode & S_IFDIR));
}

void Logger::createLogDir()
{
    if (!logDirExists())
    {
        if (mkdir(logDir.c_str(), 0755) == -1)
        {
            cerr << "Error creating log directory: " << endl;
        }
    }
}

Logger::Logger()
{
    logDir = getENV("LOG_DIR");
    if (logDir.empty())
        logDir = "./logs";
    if (logDir.back() != '/')
        logDir += '/';
    createLogDir();
}

void Logger::log(const string &message, const string &filename)
{
    string fullPath = logDir + filename;
    ofstream logFile(fullPath, ios::app);
    if (logFile.is_open())
    {
        logFile << getCurrentTime() << " - " << message << endl;
    }
    else
    {
        cerr << "Cannot open log file: " << fullPath << endl;
    }
}

string Logger::getLogDir() const
{
    return logDir;
}


