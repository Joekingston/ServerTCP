#include "Logging.h"

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }
#endif
    string test = R"({
        "APP": "myTest",
        "LEVEL": "testLevel",
        "LOG": "logTest"})"; 
    string test2;
    Logging loggerServer;
    loggerServer.parseAndFormatLog(test, test2);
    printf("%s", test2.c_str());
    test = R"({
        "APP": "myTest",
        "LEVEL": "testLevel"})";
    loggerServer.parseAndFormatLog(test, test2);
    printf("%s", test2.c_str());
    loggerServer.startListening();


    return 0;
}

