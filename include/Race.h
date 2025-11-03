#ifndef PROJEKT_RACE_H
#define PROJEKT_RACE_H

#include <vector>
#include "Car.h"
#include "Track.h"

class Race {
public:
    Race(Track& track, std::vector<Car*>& cars);
    void run();
private:
    void printResults();

    Track& raceTrack;
    std::vector<Car*> raceCars;
    double totalTime = 0.0;
    double totalTrackLength = 0.0;
    std::vector<std::pair<std::string, double>> results;
};

#endif //PROJEKT_RACE_H
