#pragma once
#include "Car.h"
#include <vector>

class Race {
public:
    Race(const std::vector<Car>& cars, const Track& track, int totalLaps);
    
    void run();

private:
    std::vector<Car> cars;
    Track track;
    int totalLaps;
    double raceTime = 0.0;
    bool raceFinished = false;

    void updateLeaderboard();
    void printStatus();
};
