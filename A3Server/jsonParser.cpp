//json breaks sockets, had to separate the files to make it proper.
#include <nlohmann/json.hpp>
#include "Logging.h"
using namespace std;

void Logging::parseAndFormatLog(const string& jsonMessage, string& formattedLog) {
    try {
        nlohmann::json j = nlohmann::json::parse(jsonMessage);

        bool appPresent = j.find("APP") != j.end();
        bool levelPresent = j.find("LEVEL") != j.end();
        bool logPresent = j.find("LOG") != j.end();

        if (appPresent && levelPresent && logPresent) {
            formattedLog = jsonMessage;
        }
        else {
            nlohmann::json reformattedLog;
            reformattedLog["APP"] = j.value("APP", "MissingApp");
            reformattedLog["LEVEL"] = j.value("LEVEL", "MissingLevel");
            reformattedLog["LOG"] = j.value("LOG", "MissingLog");
            formattedLog = reformattedLog.dump();           
        }
        
    }
    catch (const exception& e) {
        perror("Error parsing JSON\n");

    }
}