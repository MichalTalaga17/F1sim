#pragma once
#include "DataStructures.h"
#include <map>
#include <string>
#include <vector>

class ConfigParser {
public:
    struct GlobalConfig {
        int laps;
        double overtakeProximity;
        double eventProbability;
        double pitStopMinDuration;
        double pitStopMaxDuration;
        // ... inne parametry globalne
    };

    ConfigParser();
    bool loadMainConfig(const std::string& filepath);
    Track loadTrack(const std::string& filepath);

    std::vector<Driver> getDrivers() const;
    std::vector<Team> getTeams() const;
    GlobalConfig getGlobalConfig() const;

private:
    std::map<std::string, Team> teamsMap;
    std::vector<Driver> driversList;
    GlobalConfig globalConfig;
    
    // Helper do parsowania linii CSV
    std::vector<std::string> split(const std::string& s, char delimiter);
};
