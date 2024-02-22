#pragma once

//Collaboration with Silas Springer, he's my friend from Uni in OHIO, he said its proper to
// include these for support on both unix and windows
#ifdef _WIN32
#include <winsock2.h>
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
#include <WS2tcpip.h>

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

    int serverSocket;
    int port = 30001;
    string logFile = "testlog.txt";
    mutex mutexWriter;
    unordered_map<string, ClientDetails> clientDetailsMap;
    mutex clientDetailsMutex;

public:

    Logging();
    ~Logging();
    void startListening();
    void handleClient(int clientSocket);
    void writeLog(const string &log);
    int checkClient(const char* ip, int clientSocket);



};