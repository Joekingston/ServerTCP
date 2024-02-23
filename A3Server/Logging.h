#pragma once

//Collaboration with Silas Springer, he's my friend from Uni in OHIO, he said its proper to
// include these for support on both unix and windows
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <WS2tcpip.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <chrono>
#include <unordered_map>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include "mutex"
#include <unordered_set>

using namespace std;

#define RATE_LIMIT_SPAM 1
#define RATE_LIMIT 8
#define RATE_LIMIT_TIME 600 //10 mins
#define PROTOCOL 0

struct ClientDetails {
    string logFormat;
    int messageCount;
    chrono::time_point<chrono::system_clock> lastMessageTime;
};

class Logging {
private:

    unordered_map<std::string, bool> logLevels;
    vector<std::thread> threads;
    int serverSocket;
    int port = 30001;
    string logFile = "testlog.txt";
    mutex mutexWriter;
    unordered_map<string, ClientDetails> clientDetailsMap;
    mutex clientDetailsMutex;
    string dateFormat;
    unordered_set<char> usedOptions;

public:

    Logging();
    ~Logging();
    void ui();
    bool isValidOption(char option) {
        if (option == 'Y' || option == 'y' || option == 'm' || option == 'd' ||
            option == 'H' || option == 'M' || option == 'S' ||
            option == 'a' || option == 'b') {
            return true;
        }

        return false;
    }
    void handleBlockLevelOption();
    void handleTimeOption();
    void startListening();
    void handleClient(int clientSocket);
    void writeLog(const string &log);
    void parseAndFormatLog(const string& jsonMessage, string& formattedLog, const string ip);
    int checkClient(const char* ip, int clientSocket);
    string toUpper(const string& str);


};