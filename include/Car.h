#ifndef PROJEKT_CAR_H
#define PROJEKT_CAR_H

#include "Track.h"

class Car {
public:
    Car();
    void update(double deltaTime);
    double getDistance() const;
    void setTrack(const Track* track);

private:
    const Track* currentTrack = nullptr;
    double distanceOnTrack = 0.0;
    double speed = 50.0; // m/s, constant for now
};

#endif //PROJEKT_CAR_H
