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

#define MIN_NUM 0
#define PROTOCOL 0

struct ClientDetails {
    string logFormat;
    int messageCount;
    chrono::time_point<chrono::system_clock> lastMessageTime;
};

class Logging {
private:

    int rateLimitSpam = 1;
    int rateLimit = 8;
    int rateLimitTime = 600;
    unordered_map<string, bool> logLevels;
    vector<std::thread> threads;
    int serverSocket;
    int port;
    string logFile = "testlog.txt";
    mutex mutexWriter;
    unordered_map<string, ClientDetails> clientDetailsMap;
    mutex clientDetailsMutex;
    string dateFormat;
    unordered_set<char> usedOptions;

public:

    Logging(int initialPort);
    ~Logging();
    void ui();
    bool isValidOption(char option);
    void handleBlockLevelOption();
    void handleTimeOption();
    void startListening();
    void handleClient(int clientSocket);
    void writeLog(const string &log);
    void parseAndFormatLog(const string& jsonMessage, string& formattedLog, const string ip);
    int checkClient(const char* ip, int clientSocket);
    void displayUI();
    void changeRateLimiting();
    string toUpper(const string& str);


};