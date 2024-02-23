#include "Logging.h"

#define FAIL -1
#define SUCCESSFUL 0

Logging::Logging() : logLevels{
        {"DEBUG", true},
        {"WARNING", true},
        {"INFO", true},
        {"CRITICAL", true},
        {"ERROR", true},
        {"NOTICE", true}}
{
    dateFormat = "%Y-%m-%d %H:%M:%S";
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

    string formattedLog = "";
    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);
    parseAndFormatLog(buffer, formattedLog, ip);
    if (!formattedLog.empty()) {
        writeLog(formattedLog);
    }

    printf("%s\n", formattedLog.c_str());
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

void Logging::handleTimeOption() {

}

void Logging::handleBlockLevelOption() {
}

void Logging::ui() {
    printf("Logger Server UI\n");

    while (true) {
        printf("Options:\n");
        printf("  1. Time\n");
        printf("  2. BlockLevel\n");
        printf("  3. Quit\n");

        int choice;
        printf("Enter your choice: ");
        if (scanf_s("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
        case 1:
            handleTimeOption();
            break;
        case 2:
            handleBlockLevelOption();
            break;
        case 3:
            printf("Exiting Logger Server UI.\n");
            return;
        default:
            printf("Invalid choice please try again.\n");
        }
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