#ifndef PROJEKT_CAR_H
#define PROJEKT_CAR_H

#include <string>
#include "Track.h"
#include "Team.h"
#include "Driver.h"

class Car {
public:
    explicit Car(const Driver& driver, const Team& team, double initialFuel);
    void update(double deltaTime, double weatherGrip);
    double getDistance() const;
    void setTrack(const Track* track);
    const std::string& getName() const;
    bool hasFinished(double trackLength) const;
    double getFuel() const;
    bool isOutOfFuel() const;

private:
    // Identity
    const Driver& driver;
    const Team& team;

    // Pre-calculated effective parameters
    double effectiveTopSpeed;
    double effectiveAcceleration;
    double effectiveBraking;
    double effectiveTireGrip;
    double effectiveFuelConsumption;

    // Physics & State
    const Track* currentTrack = nullptr;
    double distanceOnTrack = 0.0;
    double currentSpeed = 0.0; // m/s
    bool outOfFuel = false;

    // Parameters
    double fuel; // in kg
    double baseMass = 798.0; // kg
};

#endif //PROJEKT_CAR_H
