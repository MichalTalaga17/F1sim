#pragma once
#include "DataStructures.h"
#include <vector>

#include <random>

class Car {
public:
    Car(const Driver& driver, const Team& team, int startGridPosition);

    void update(double dt, const Track& track, int totalLaps, WeatherType weather, std::mt19937& rng, const Car* carAhead = nullptr);

    double getTotalDistance() const { return totalDistance; }
    double getLapDistance() const { return lapDistance; }
    double getCurrentSpeed() const { return currentSpeed; }
    int getCurrentLap() const { return currentLap; }
    bool hasFinished(int totalLaps) const { return finished; }
    const Driver& getDriver() const { return driver; }
    const Team& getTeam() const { return team; }

    bool isPitting() const { return pitStopTimer > 0; }
    std::string getStatus() const;

    void completeRace(double time) { 
        finished = true; 
        finishTime = time;
    }
    double getFinishTime() const { return finishTime; }

private:
    Driver driver;
    Team team;

    double totalDistance = 0.0;
    double lapDistance = 0.0;
    double currentSpeed = 0.0;
    int currentLap = 1;
    bool finished = false;
    double finishTime = 0.0;

    double tireHealth = 1.0;
    double lapStartTireHealth = 1.0;
    double tireWearPerLap = 0.02;
    double fuelLoad = 10.0;

    double pitStopTimer = 0.0;

    double calculateTargetSpeed(const TrackSegment& segment, double gripModifier);
    double getEffectiveAcceleration(double gripModifier);
    double getEffectiveBraking(double gripModifier);
};
