#pragma once
/*FILE: Logging.h
 PROJECT : SENG2040 A3
 PROGRAMMER : Joseph Colby Carson 8213035
 FIRST VERSION : 2024 / 02 /23
 DESCRIPTION: THIS Does not use linux, I didn't have the time to try and recreate
 This houses the logging class with all the prototypes and variables this class launches a server dedicated to collecting logs from applications and moving them into a file*/
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


//Name    : Logging
//Purpose : This houses all the logging class function, its used to run a server and take clients logs, which sends the logs
// into text files 
class Logging {
private:

    int rateLimitSpam = 1; // default values
    int rateLimit = 8; // default value
    int rateLimitTime = 600; //defualt value 10 mins
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
    Logging();
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