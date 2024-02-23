/*FILE: JsonParser.cpp
 PROJECT : SENG2040 A3
 PROGRAMMER : Joseph Colby Carson 8213035
 FIRST VERSION : 2024 / 02 /23
 DESCRIPTION: This file was needed to be separated, it houses the parsing of the json files and returns the formatted strings via reference*/

//json breaks sockets, had to separate the files to make it proper.
#include <nlohmann/json.hpp>
#include "Logging.h"
#include <ctime>
#define ERROR_CHECK 0
using namespace std;

// Name    : parseAndFormatLog
// Purpose : Parses a JSON log message, formats it according to specified criteria, and assigns the formatted log.
// Inputs  : const string& jsonMessage : The JSON log message to parse, string& formattedLog : Output parameter for the formatted log, const string ip : The client's IP address.
// Returns : void
void Logging::parseAndFormatLog(const string& jsonMessage, string& formattedLog, const string ip) {

    try {
        nlohmann::json j = nlohmann::json::parse(jsonMessage);

        bool appPresent = (j.find("APP") != j.end()) && !j["APP"].get<string>().empty();
        bool levelPresent = (j.find("LEVEL") != j.end()) && !j["LEVEL"].get<string>().empty();
        bool logPresent = (j.find("LOG") != j.end()) && !j["LOG"].get<string>().empty();

        if (levelPresent) {
            string level = toUpper(j["LEVEL"].get<string>());
            if (logLevels.find(level) != logLevels.end() && !logLevels[level]) {
                printf("bounced");  return;
            }
            time_t now = time(MIN_NUM);
            struct tm localTimeInfo;
            if (localtime_s(&localTimeInfo, &now) != ERROR_CHECK) {
                printf("Error getting local time\n");
                return;
            }
            char dt[30];
            //removes new line
            strftime(dt, sizeof(dt), dateFormat.c_str(), &localTimeInfo);
            size_t length = strlen(dt);
            if (length > MIN_NUM && dt[length - 1] == '\n') {
                dt[length - 1] = '\0';
            }
            //
            if (appPresent && levelPresent && logPresent) {
                formattedLog = string(dt) + " " +
                    ip + " " +
                    level + " " +
                    j["APP"].get<string>() + ": " +
                    j["LOG"].get<string>();
            }
            else {

                formattedLog = string(dt) + " " +
                    ip + " " +
                    level + " " +
                    j.value("APP", "MissingApp") + " " +
                    j.value("LOG", "MissingLog");

            }
            
           

        }
        else return;

    }
    catch (const exception& e) {
        perror("Error parsing JSON \n");
    }
}

// Name    : toUpper
// Purpose : Converts a string to uppercase.
// Inputs  : const string& str : The input string.
// Returns : string : The input string converted to uppercase.
string Logging::toUpper(const string& str) {
    string upperStr = str;
    transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}