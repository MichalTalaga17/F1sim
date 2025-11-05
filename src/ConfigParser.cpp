#include "ConfigParser.h"
#include "Driver.h"
#include "Team.h"
#include <fstream>
#include <sstream>
#include <algorithm>

// Helper function to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" 	");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" 	");
    return str.substr(first, (last - first + 1));
}
std::vector<std::string> ConfigParser::split(const std::string& s, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

bool ConfigParser::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::string currentSection;
    std::vector<std::string> sectionLines;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue; // Skip empty lines and comments
        }

        if (line[0] == '[' && line.back() == ']') {
            if (!currentSection.empty()) {
                parseSection(currentSection, sectionLines);
                sectionLines.clear();
            }
            currentSection = line.substr(1, line.length() - 2);
        } else {
            sectionLines.push_back(line);
        }
    }
    // Parse the last section
    if (!currentSection.empty()) {
        parseSection(currentSection, sectionLines);
    }

    return true;
}

void ConfigParser::parseSection(const std::string& sectionName, const std::vector<std::string>& lines) {
    if (sectionName == "Drivers") {
        parseDrivers(lines);
    } else if (sectionName == "Teams") {
        parseTeams(lines);
    } else if (sectionName == "DriverToTeam") {
        parseDriverToTeam(lines);
    } else {
        for (const std::string& line : lines) {
            size_t eqPos = line.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(line.substr(0, eqPos));
                std::string value = trim(line.substr(eqPos + 1));
                configData[sectionName][key] = value;
            }
        }
    }
}

void ConfigParser::parseDrivers(const std::vector<std::string>& lines) {
    for (const std::string& line : lines) {
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() == 5) {
            Driver d;
            d.name = tokens[0];
            d.exp = std::stoi(tokens[1]);
            d.rac = std::stoi(tokens[2]);
            d.awa = std::stoi(tokens[3]);
            d.pac = std::stoi(tokens[4]);
            drivers[d.name] = d;
        }
    }
}

void ConfigParser::parseTeams(const std::vector<std::string>& lines) {
    for (const std::string& line : lines) {
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() == 5) {
            Team t;
            t.name = tokens[0];
            t.topSpeed = std::stod(tokens[1]);
            t.accelerationRate = std::stod(tokens[2]);
            t.brakingRate = std::stod(tokens[3]);
            t.baseTireGrip = std::stod(tokens[4]);
            teams[t.name] = t;
        }
    }
}

void ConfigParser::parseDriverToTeam(const std::vector<std::string>& lines) {
    for (const std::string& line : lines) {
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() == 2) {
            driverToTeam[tokens[0]] = tokens[1];
        }
    }
}

std::string ConfigParser::getString(const std::string& section, const std::string& key) const {
    auto itSection = configData.find(section);
    if (itSection != configData.end()) {
        auto itKey = itSection->second.find(key);
        if (itKey != itSection->second.end()) {
            return itKey->second;
        }
    }
    throw std::runtime_error("Config key not found: " + section + ":" + key);
}

int ConfigParser::getInt(const std::string& section, const std::string& key) const {
    return std::stoi(getString(section, key));
}

double ConfigParser::getDouble(const std::string& section, const std::string& key) const {
    return std::stod(getString(section, key));
}