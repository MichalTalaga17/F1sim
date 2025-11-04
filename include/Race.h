#ifndef PROJEKT_RACE_H
#define PROJEKT_RACE_H

#include <vector>
#include "Car.h"
#include "Track.h"
#include "Weather.h"
#include "ConfigParser.h"
#include <map>
#include <random>

struct RaceResult {
    std::string carName;
    double time;
    std::string strategy;
    int positionChange;
    double totalPitStopTime;
};

class Race {
public:
    Race(Track& track, std::vector<Car*>& cars, const ConfigParser& config);
    void run();
    void attemptOvertake(Car* attacker, Car* defender);
private:
    void printResults();

    Track& raceTrack;
    std::vector<Car*> raceCars;
    Weather weather;
    int raceLaps;
    std::vector<int> carLaps;
    double totalTime = 0.0;
    double totalTrackLength = 0.0;
    std::vector<RaceResult> results;
    std::map<std::string, int> startingPositions;

    std::mt19937 rng;
    std::uniform_real_distribution<double> pitStopTimeDistribution;

    // Event related distributions
    std::uniform_real_distribution<double> eventProbabilityDistribution;
    std::uniform_int_distribution<int> carSelectionDistribution;
    std::uniform_int_distribution<int> eventTypeDistribution;
    std::uniform_real_distribution<double> eventDurationDistribution;
    std::uniform_real_distribution<double> overtakeProbabilityDistribution;
};

#endif //PROJEKT_RACE_H
