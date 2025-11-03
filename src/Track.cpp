#include "Track.h"
#include <fstream>
#include <iostream>
#include <sstream>

Straight::Straight(double length) : length(length) {}

double Straight::getLength() const {
    return length;
}

Corner::Corner(double radius, double angle) : radius(radius), angle(angle) {}

double Corner::getLength() const {
    // Arc length formula: 2 * PI * r * (angle / 360)
    return 2 * 3.1415926535 * radius * (angle / 360.0);
}

bool Track::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open track file: " << filePath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string type;
        ss >> type;
        if (type == "STRAIGHT") {
            double length;
            ss >> length;
            segments.push_back(std::make_unique<Straight>(length));
        } else if (type == "CORNER") {
            double radius, angle;
            ss >> radius >> angle;
            segments.push_back(std::make_unique<Corner>(radius, angle));
        }
    }
    return true;
}

const std::vector<std::unique_ptr<TrackSegment>>& Track::getSegments() const {
    return segments;
}
