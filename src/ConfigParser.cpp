#include "../include/ConfigParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <filesystem>

ConfigParser::ConfigParser() {}

std::vector<std::string> ConfigParser::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool ConfigParser::loadMainConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "CRITICAL ERROR: Cannot open config file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    std::string currentSection;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line[0] == '[') {
            size_t end = line.find(']');
            if (end != std::string::npos) {
                currentSection = line.substr(1, end - 1);
            }
            continue;
        }

        auto tokens = split(line, ',');
        
        if (currentSection == "Race") {
             if (line.find("Laps=") != std::string::npos) globalConfig.laps = std::stoi(line.substr(5));
        }
        else if (currentSection == "Teams") {
            if (tokens.size() >= 6) {
                Team t;
                t.name = tokens[0];
                t.topSpeed = std::stod(tokens[1]);
                t.acceleration = std::stod(tokens[2]);
                t.braking = std::stod(tokens[3]);
                t.baseTireGrip = std::stod(tokens[4]);
                t.pitStopMultiplier = std::stod(tokens[5]);
                teamsMap[t.name] = t;
            }
        }
        else if (currentSection == "Drivers") {
            if (tokens.size() >= 5) {
                Driver d;
                d.name = tokens[0];
                d.exp = std::stoi(tokens[1]);
                d.racecraft = std::stoi(tokens[2]);
                d.awareness = std::stoi(tokens[3]);
                d.pace = std::stoi(tokens[4]);
                driversList.push_back(d);
            }
        }
        else if (currentSection == "DriverToTeam") {
            if (tokens.size() >= 2) {
                std::string dName = tokens[0];
                std::string tName = tokens[1];
                for (auto& d : driversList) {
                    if (d.name == dName) {
                        d.teamName = tName;
                        break;
                    }
                }
            }
        }
    }
    return true;
}

Track ConfigParser::loadTrack(const std::string& filepath) {
    Track track;
    track.filePath = filepath;
    std::filesystem::path p(filepath);
    track.name = p.stem().string(); 
    track.totalLength = 0;

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error loading track: " << filepath << std::endl;
        return track;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string type;
        ss >> type;

        TrackSegment seg;
        seg.type = type;
        
        if (type == "STRAIGHT") {
            ss >> seg.length;
            seg.angle = 0;
            seg.radius = 0;
        } else if (type == "CORNER") {
            ss >> seg.angle >> seg.radius;
            seg.length = (std::abs(seg.angle) / 360.0) * 2 * 3.14159 * seg.radius;
        }
        
        track.segments.push_back(seg);
        track.totalLength += seg.length;
    }
    return track;
}

std::vector<Driver> ConfigParser::getDrivers() const { return driversList; }
std::vector<Team> ConfigParser::getTeams() const {
    std::vector<Team> teams;
    for(auto const& [name, team] : teamsMap) {
        teams.push_back(team);
    }
    return teams;
}
ConfigParser::GlobalConfig ConfigParser::getGlobalConfig() const { return globalConfig; }
