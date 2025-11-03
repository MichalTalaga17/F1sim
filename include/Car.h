#ifndef PROJEKT_CAR_H
#define PROJEKT_CAR_H

#include <string>
#include "Track.h"

class Car {
public:
    explicit Car(std::string name, double speed);
    void update(double deltaTime);
    double getDistance() const;
    void setTrack(const Track* track);
    const std::string& getName() const;
    bool hasFinished(double trackLength) const;

private:
    std::string name;
    const Track* currentTrack = nullptr;
    double distanceOnTrack = 0.0;
    double speed; // m/s
};

#endif //PROJEKT_CAR_H
