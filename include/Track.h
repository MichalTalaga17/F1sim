#ifndef PROJEKT_TRACK_H
#define PROJEKT_TRACK_H

#include <vector>
#include <string>
#include <memory>
#include "Vector2D.h"

class TrackSegment {
public:
    virtual ~TrackSegment() = default;
    virtual double getLength() const = 0;
};

class Straight : public TrackSegment {
public:
    explicit Straight(double length);
    double getLength() const override;
private:
    double length;
};

class Corner : public TrackSegment {
public:
    Corner(double radius, double angle);
    double getLength() const override;
private:
    double radius;
    double angle; // in degrees
};

class Track {
public:
    Track() = default;
    bool loadFromFile(const std::string& filePath);
    const std::vector<std::unique_ptr<TrackSegment>>& getSegments() const;
private:
    std::vector<std::unique_ptr<TrackSegment>> segments;
};

#endif //PROJEKT_TRACK_H
