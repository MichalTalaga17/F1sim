#pragma once
#include <string>
#include <vector>

enum class WeatherType {
    Sunny,
    Cloudy,
    Rainy,
    Storm
};

struct Team {
    std::string name;
    double topSpeed;
    double acceleration;
    double braking;
    double baseTireGrip;
    double pitStopMultiplier;
};

struct Driver {
    std::string name;
    int exp;
    int racecraft;
    int awareness;
    int pace;
    std::string teamName;
};

struct TrackSegment {
    std::string type; // STRAIGHT or CORNER
    double length;
    double angle;  // For corners
    double radius; // For corners
};

struct Track {
    std::string name;
    std::string filePath;
    std::vector<TrackSegment> segments;
    double totalLength;
};
