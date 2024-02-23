#include "Logging.h"

#define FAIL -1
#define SUCCESSFUL 0

Logging::Logging()
{
    serverSocket = -1;
}

Logging::~Logging()
{
    WSACleanup();
}



void Logging::handleClient(int clientSocket) {

    sockaddr_in clientAddr;
    int addrSize = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr*)&clientAddr, &addrSize);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, INET_ADDRSTRLEN);
    lock_guard<mutex> lock(clientDetailsMutex);
    if (int Result = checkClient(ip, clientSocket) == FAIL){
        return;
    }
    auto currentTime = chrono::system_clock::now();
    auto timeDifference = chrono::duration_cast<chrono::seconds>(currentTime - clientDetailsMap[ip].lastMessageTime);

    if (timeDifference.count() <= RATE_LIMIT_SPAM) {
        clientDetailsMap[ip].messageCount++;
    }
    else clientDetailsMap[ip].messageCount = 0;
    string test, test2;
    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);
    parseAndFormatLog(buffer, test2);
    writeLog(test2);
    printf("%s %s\n", test2.c_str(), ip);
    clientDetailsMap[ip].lastMessageTime = currentTime;
#ifdef _WIN32 // end of unix support
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

int Logging::checkClient(const char* ip, int clientSocket) {
    if (clientDetailsMap.find(ip) == clientDetailsMap.end()) {
        clientDetailsMap[ip] = { "FixForLater", 0, chrono::system_clock::now() };
    }
    else if (clientDetailsMap[ip].messageCount >= RATE_LIMIT) {

        auto currentTime = chrono::system_clock::now();
        auto timeDifference = chrono::duration_cast<chrono::seconds>(currentTime - clientDetailsMap[ip].lastMessageTime);

        if (timeDifference.count() <= RATE_LIMIT_TIME) {
            printf("Rate limited user: %s\n", ip);
        #ifdef _WIN32 
            closesocket(clientSocket);
        #else
            close(clientSocket);
        #endif
            return FAIL;
        }
        else {
            clientDetailsMap[ip].messageCount = 0;
        }
    }
    return SUCCESSFUL;
}


void Logging::startListening() {

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);


    serverSocket = socket(serverAddress.sin_family, SOCK_STREAM, PROTOCOL);
    if (serverSocket == -1) {
        perror("Error creating socket\n");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding socket\n");
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        perror("Listen failed with error\n");
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Listening for logging messages on port %d\n", port);

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        printf("Client Connecting...\n");  //Log here later. nvm
        if (clientSocket < 0) {
            perror("Error accepting connection\nListening\n");
            continue;
        }

        thread clientThread(&Logging::handleClient, this, clientSocket);
        clientThread.detach();
    }

}



void Logging::writeLog(const string &log) {
    lock_guard<mutex> lock(mutexWriter); 
    ofstream logFile("testlog.txt", ios_base::app);
    if (logFile.is_open()) {
        logFile.write(log.c_str(), log.length());
        logFile.put('\n');
        logFile.close();
    }
    else {
        printf("Error opening log file\n");
    }
}