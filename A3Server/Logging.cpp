#include "Logging.h"


Logging::Logging()
{
    serverSocket = -1;
}

Logging::~Logging()
{
}

void Logging::handleClient(int clientSocket) {

    sockaddr_in clientAddr;
    int addrSize = sizeof(clientAddr);
    getpeername(clientSocket, (struct sockaddr*)&clientAddr, &addrSize);
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, INET_ADDRSTRLEN);

    if (clientDetailsMap.find(ip) == clientDetailsMap.end()) {
        clientDetailsMap[ip] = { "FixForLater", 0, chrono::system_clock::now() };
    }

    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);
    writeLog(buffer);
    printf("%s%s", buffer, ip);

#ifdef _WIN32 // end of unix support
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

void Logging::startListening() {

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);


    serverSocket = socket(serverAddress.sin_family, SOCK_STREAM, PROTOCOL);
    if (serverSocket == -1) {
        perror("Error creating socket");
        WSACleanup();
        exit(EXIT_FAILURE);
    }


    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding socket");
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        perror("Listen failed with error");
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Listening for logging messages on port %d\n", port);

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        printf("Client Connecting...");  //Log here later. nvm
        if (clientSocket < 0) {
            perror("Error accepting connection\nListening");
            continue;
        }

        thread clientThread(&Logging::handleClient, this, clientSocket);
        clientThread.detach(); 
    }

}

void Logging::writeLog(const string &log) {
    lock_guard<std::mutex> lock(mutex); 
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