#ifndef PROJEKT_CONFIGPARSER_H
#define PROJEKT_CONFIGPARSER_H

#include <string>
#include <map>
#include <vector>
#include <stdexcept>

// Forward declarations for Driver and Team structs
struct Driver;
struct Team;

class ConfigParser {
public:
    ConfigParser() = default;
    bool loadFromFile(const std::string& filePath);

    // Generic getters for section and key
    std::string getString(const std::string& section, const std::string& key) const;
    int getInt(const std::string& section, const std::string& key) const;
    double getDouble(const std::string& section, const std::string& key) const;

    // Specific getters for structured data
    const std::map<std::string, Driver>& getDrivers() const { return drivers; }
    const std::map<std::string, Team>& getTeams() const { return teams; }
    const std::map<std::string, std::string>& getDriverToTeamMap() const { return driverToTeam; }

private:
    std::map<std::string, std::map<std::string, std::string>> configData;
    std::map<std::string, Driver> drivers;
    std::map<std::string, Team> teams;
    std::map<std::string, std::string> driverToTeam;

    void parseSection(const std::string& sectionName, const std::vector<std::string>& lines);
    void parseDrivers(const std::vector<std::string>& lines);
    void parseTeams(const std::vector<std::string>& lines);
    void parseDriverToTeam(const std::vector<std::string>& lines);

    // Helper to split a string by delimiter
    std::vector<std::string> split(const std::string& s, char delimiter) const;
};

#endif //PROJEKT_CONFIGPARSER_H
