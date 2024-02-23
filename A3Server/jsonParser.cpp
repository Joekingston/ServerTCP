//json breaks sockets, had to separate the files to make it proper.
#include <nlohmann/json.hpp>
#include "Logging.h"
#include <ctime>
using namespace std;

void Logging::parseAndFormatLog(const string& jsonMessage, string& formattedLog, const string ip) {

    try {
        nlohmann::json j = nlohmann::json::parse(jsonMessage);

        bool appPresent = j.find("APP") != j.end();
        bool levelPresent = j.find("LEVEL") != j.end();
        bool logPresent = j.find("LOG") != j.end();

        if (levelPresent) {
            string level = toUpper(j["LEVEL"].get<std::string>());
            if (logLevels.find(level) != logLevels.end() && !logLevels[level]) {
                printf("bounced");  return;
            }
            dateFormat = "%a %b %d %H:%M:%S %Y";
            time_t now = time(0);
            struct tm localTimeInfo;
            if (localtime_s(&localTimeInfo, &now) != 0) {
                printf("Error getting local time\n");
                return;
            }
            char dt[30];
            strftime(dt, sizeof(dt), dateFormat.c_str(), &localTimeInfo);
            size_t length = std::strlen(dt);
            if (length > 0 && dt[length - 1] == '\n') {
                dt[length - 1] = '\0';
            }
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

string Logging::toUpper(const string& str) {
    string upperStr = str;
    transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}