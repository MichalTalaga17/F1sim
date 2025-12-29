#pragma once
#include "DataStructures.h"
#include <vector>

class Car {
public:
    Car(const Driver& driver, const Team& team);

    // Główna metoda aktualizacji stanu (wywoływana co np. 0.1s)
    void update(double dt, const Track& track, WeatherType weather);

    // Gettery
    double getTotalDistance() const { return totalDistance; }
    double getLapDistance() const { return lapDistance; }
    double getCurrentSpeed() const { return currentSpeed; } // m/s
    int getCurrentLap() const { return currentLap; }
    bool hasFinished(int totalLaps) const { return finished; }
    const Driver& getDriver() const { return driver; }
    const Team& getTeam() const { return team; }
    
    // Status
    bool isPitting() const { return pitStopTimer > 0; }
    std::string getStatus() const; // np. "Pitting", "Lap 5", "Finished"
    
    // Logic
    void completeRace(double time) { 
        finished = true; 
        finishTime = time;
    }
    double getFinishTime() const { return finishTime; }

private:
    Driver driver;
    Team team;

    // Fizyka i Pozycja
    double totalDistance = 0.0;
    double lapDistance = 0.0;
    double currentSpeed = 0.0; // m/s
    int currentLap = 1;
    bool finished = false;
    double finishTime = 0.0; // Czas ukończenia wyścigu

    // Opony i Paliwo (uproszczone na start)
    double tireHealth = 1.0; // 1.0 = 100%
    double fuelLoad = 10.0;  // kg (wpływa na masę)

    // Pit Stop
    double pitStopTimer = 0.0;

    // Helpery fizyczne
    double calculateTargetSpeed(const TrackSegment& segment, double gripModifier);
    double getEffectiveAcceleration(double gripModifier);
    double getEffectiveBraking(double gripModifier);
};
