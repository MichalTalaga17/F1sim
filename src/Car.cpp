#include "Car.h"
#include <iostream>
#include <cmath> // For std::sqrt and std::min

Car::Car(const Driver& driver, const Team& team, double initialFuel)
    : driver(driver), team(team), fuel(initialFuel) {

    // Pre-calculate effective parameters based on driver skills and team stats
    double paceFactor = 1.0 + (driver.pac - 85.0) * 0.0025;
    double expFactor = 1.0 - (driver.exp - 85.0) * 0.001;

    effectiveTopSpeed = team.topSpeed * (1.0 + (driver.pac - 85.0) * 0.001); // Pace affects top speed
    effectiveAcceleration = team.accelerationRate * paceFactor;
    effectiveBraking = team.brakingRate;
    effectiveTireGrip = team.baseTireGrip * paceFactor; // Pace affects cornering
    effectiveFuelConsumption = 0.1 * expFactor; // Experience affects fuel saving
}

void Car::setTrack(const Track* track) {
    currentTrack = track;
}

void Car::update(double deltaTime, double weatherGrip) {
    if (!currentTrack) return;

    // Determine target speed
    double targetSpeed = effectiveTopSpeed;

    if (isOutOfFuel()) {
        targetSpeed = 0.0;
    } else {
        double distanceIntoSegment;
        const TrackSegment* currentSegment = currentTrack->getSegmentAt(distanceOnTrack, distanceIntoSegment);

        if (const auto* corner = dynamic_cast<const Corner*>(currentSegment)) {
            double radius = corner->getRadius();
            double totalGrip = effectiveTireGrip * weatherGrip;
            double maxCornerSpeed = std::sqrt(totalGrip * 9.81 * radius);
            targetSpeed = std::min(effectiveTopSpeed, maxCornerSpeed);
        }
    }

    // Accelerate or brake towards target speed
    if (currentSpeed < targetSpeed) {
        currentSpeed += effectiveAcceleration * deltaTime;
        currentSpeed = std::min(currentSpeed, targetSpeed);
    } else if (currentSpeed > targetSpeed) {
        currentSpeed -= effectiveBraking * deltaTime;
        currentSpeed = std::max(currentSpeed, targetSpeed);
    }

    // Update distance and fuel
    double distanceMoved = currentSpeed * deltaTime;
    distanceOnTrack += distanceMoved;

    if (!isOutOfFuel()) {
        fuel -= (currentSpeed / effectiveTopSpeed) * effectiveFuelConsumption * deltaTime;
        if (fuel <= 0) {
            fuel = 0;
            outOfFuel = true;
            std::cout << "[EVENT] " << driver.name << " has run out of fuel!" << std::endl;
        }
    }
}

double Car::getDistance() const {
    return distanceOnTrack;
}

const std::string& Car::getName() const {
    return driver.name;
}

bool Car::hasFinished(double trackLength) const {
    return distanceOnTrack >= trackLength;
}

double Car::getFuel() const {
    return fuel;
}

bool Car::isOutOfFuel() const {
    return outOfFuel;
}
