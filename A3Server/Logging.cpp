#include "Logging.h"

#define FAIL -1
#define SUCCESSFUL 0
#define BUFFER_SIZE 2048
#define DATE_LENGTH 35
#define CHECK_PREVIOUS_OR_FUTURE 1

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
    else clientDetailsMap[ip].messageCount = MIN_NUM;

    string formattedLog = "";
    char buffer[BUFFER_SIZE] = { 0 };
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
        clientDetailsMap[ip] = { "FixForLater", MIN_NUM, chrono::system_clock::now() };
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
    if (serverSocket == FAIL) {
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

bool Logging::isValidOption(char option) {

    if (option == 'Y' || option == 'y' || option == 'm' || option == 'd' ||
        option == 'H' || option == 'M' || option == 'S' || option == 'a' ||
        option == 'b') {
        return true;
    }
    return false;
}
void Logging::displayUI() {

    printf("Logger Server UI\n");
    printf("Options:\n");
    printf("  1. Time\n");
    printf("  2. Block Levels\n");
    printf("  3. Display UI\n");
}


void Logging::handleTimeOption() {

    printf("Options:\n");
    printf("Year, Month, and Day:\n");
    printf("  %%Y: Year with century as a decimal number (e.g., 2024).\n");
    printf("  %%y: Year without century as a decimal number (00-99).\n");
    printf("  %%m: Month as a zero-padded decimal number (01-12).\n");
    printf("  %%d: Day of the month as a zero-padded decimal number (01-31).\n");
    printf("Example: %%Y-%%m-%%d = \"2024-02-23\".\n\n");

    printf("Hour, Minute, and Second:\n");
    printf("  (%%H:): Hour (00-23).\n");
    printf("  (%%M:): Minute (00-59).\n");
    printf("  %%S: Second (00-59).\n");
    printf("Example: %%H:%%M:%%S = \"13:24:45\".\n\n");

    printf("Day of the Week and Month Name:\n");
    printf("  %%a: Abbreviated weekday name (Sun, Mon, Tue, etc.).\n");
    printf("  %%b: Abbreviated month name (Jan, Feb, Mar, etc.).\n");
    char newFormat[DATE_LENGTH] = "";
    cin.ignore(); 
    printf("Enter the new date format: ");
    cin.getline(newFormat, sizeof(newFormat));
    usedOptions.clear();
    for (size_t i = 0; i < strlen(newFormat); ++i) {
        char option = newFormat[i];
        if (option == '%' || option == ':' || option == '-') {
            if (option == '-') {
                if (newFormat[i + 1] != '-') {
                    continue;
                }
            }
            if (newFormat[i + CHECK_PREVIOUS_OR_FUTURE] == '%' || newFormat[i + CHECK_PREVIOUS_OR_FUTURE] == ':') {
                printf("Error: Invalid syntax: doubled spacer\n");
                return;
            }
            continue;
        }

        if (isValidOption(option)) {
            if (usedOptions.find(option) != usedOptions.end()) {
                printf("Error: Option '%c' is repeated\n", option);
                return;
            }
            if (newFormat[i - CHECK_PREVIOUS_OR_FUTURE] != '%') { //possible invalid variable, but still checks properly so go away.
                printf("Error: Invalid syntax: Missing %%\n");
                return;
            }
            usedOptions.insert(option);
        }
        else {
            printf("Error: Invalid option '%c'\n", option);
            return;
        }
    }
    printf("New date format: %s\n", newFormat);
    dateFormat = newFormat;
    return;
}



void Logging::handleBlockLevelOption() {

    printf("Log Levels:\n");
    int index = 1;
    for (const auto& entry : logLevels) {
        printf("%d. %s: %s\n", index, entry.first.c_str(), entry.second ? "Enabled" : "Disabled");
        ++index;
    }
    printf("Enter the number of the level to toggle (0 to exit): ");
    int choice;
    if (scanf_s("%d", &choice) != 1) {
        printf("Invalid input. Please enter a valid number\n");
        return;
    }
    if (choice == 0) {
        return;
    }
    if (choice < 1 || choice > logLevels.size()) {
        printf("Invalid choice. Please enter a valid number\n");
        return;
    }
    auto it = logLevels.begin();
    advance(it, choice - 1);
    it->second = !it->second;

    printf("%s is now %s\n", it->first.c_str(), it->second ? "Enabled" : "Disabled");
}


void Logging::ui() {

    while (true) {
        displayUI();

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
            break;
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