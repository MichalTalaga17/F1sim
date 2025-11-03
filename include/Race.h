#ifndef PROJEKT_RACE_H
#define PROJEKT_RACE_H

#include "Car.h"
#include "Track.h"

class Race {
public:
    Race(Track& track, Car& car);
    void run();
private:
    Track& raceTrack;
    Car& raceCar;
    double totalTime = 0.0;
    double totalTrackLength = 0.0;
};

#endif //PROJEKT_RACE_H
