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
    Logging loggerServer;

    loggerServer.ui();
    loggerServer.startListening();


    return 0;
}

