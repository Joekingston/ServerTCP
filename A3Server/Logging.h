#pragma once

//Collaboration with Silas Springer, he's my friend from Uni in OHIO, he said its proper to
// include these for support on both unix and windows
#ifdef _WIN32 || _WIN64
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

#define PROTOCOL 0

class Logging {
private:

    int serverSocket;
    int port = 30001;

public:

    Logging();
    ~Logging();
    void startListening();


};