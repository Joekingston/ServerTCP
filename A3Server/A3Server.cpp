/*FILE: A3Server.cpp
 PROJECT : SENG2040 A3
 PROGRAMMER : Joseph Colby Carson 8213035
 FIRST VERSION : 2024 / 02 /23
 DESCRIPTION: this application houses a server logger which clients can connect to and submit logs, logs are timed based on received in an ordered queue, each person is identified by IP
 further application related to that ip to help narrow down issues, this application has configurable date format, disabling levels, rate limiting configurations.
 The server requires a connection and a log LEVEL thats it, this can help ensure if any critical info was missed via the level*/
#include "Logging.h"

void trimAndNullTerm(char* str);

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
    char filename[100];
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
    while (true) {
        printf("Enter a single filename less than 95 characters: ");
        if (scanf_s("%95s", filename, sizeof(filename)) != 1) {
            printf("Error reading input.\n");
            return 1; 
        }
        trimAndNullTerm(filename);
        if (strlen(filename) == 0) {
            printf("Invalid filename. Please try again.\n");
        }
        break;
    }
    system("cls");
    Logging loggerServer(port, filename); 
    thread uiThread([&]() {
        loggerServer.ui();
    });

    loggerServer.startListening();

    return 0;
}

// Name    : trimAndNullTerm
// Purpose : Handles if multiple words are processed, adds null term. 
// Inputs  : None
// Returns : void
void trimAndNullTerm(char* str) {
    while (isspace(*str)) {
        str++;
    }
    char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }
    strcat_s(str, strlen(str) + strlen(".txt") + 1, ".txt");
    strcat_s(str, strlen(str) + 1, "");
}