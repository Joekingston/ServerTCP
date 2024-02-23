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
    int port;

    while (true) {
        printf("Enter a port number: ");
        if (scanf_s("%d", &port) == 1) {
            if (port >= 1 && port <= 65535) {
                break;
            }
            else {
                printf("Invalid port number. Please enter a number between 1 and 65535.\n");
            }
        }
        else {
            printf("Invalid input. Please enter a valid integer.\n");
            while (getchar() != '\n');
        }
    }
    


    Logging loggerServer(port); 
    thread uiThread([&]() {
        loggerServer.ui();
        });

    loggerServer.startListening();


    return 0;
}

