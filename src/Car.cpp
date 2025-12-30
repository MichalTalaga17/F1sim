#include "../include/Car.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Car::Car(const Driver& d, const Team& t, int startGridPosition) : driver(d), team(t) {
    totalDistance = -static_cast<double>(startGridPosition) * 8.0;
    lapDistance = totalDistance;
    currentTire = TireCompound::Soft; // Start on Softs
    nextTire = TireCompound::Medium; 
}

void Car::update(double dt, const Track& track, WeatherType weather, std::mt19937& rng, int totalLaps, const Car* carAhead) {
    if (finished) return;

    if (pitStopTimer > 0) {
        pitStopTimer -= dt;
        if (pitStopTimer <= 0) {
            tireHealth = 1.0;
            pitStopTimer = 0;
            currentTire = nextTire; // Switch tires
        }
        return;
    }

    std::uniform_real_distribution<> dist01(0.0, 1.0);
    double mistakeProb = 0.005 * (1.0 - (driver.awareness / 100.0));
    
    bool mistakeMade = false;
    if (dist01(rng) < mistakeProb) {
        currentSpeed *= 0.85; 
        tireHealth -= 0.02;
        mistakeMade = true;
    }

    double effectiveLapDist = lapDistance;
    if (effectiveLapDist < 0) effectiveLapDist += track.totalLength;
    
    double distanceSearch = 0.0;
    const TrackSegment* currentSeg = &track.segments[0];
    const TrackSegment* nextSeg = nullptr;

    if (lapDistance >= 0) {
        for (size_t i = 0; i < track.segments.size(); ++i) {
            distanceSearch += track.segments[i].length;
            if (distanceSearch >= lapDistance) {
                currentSeg = &track.segments[i];
                if (i + 1 < track.segments.size()) nextSeg = &track.segments[i+1];
                else nextSeg = &track.segments[0]; 
                break;
            }
        }
    } else {
        currentSeg = &track.segments[0];
        nextSeg = (track.segments.size() > 1) ? &track.segments[1] : &track.segments[0];
    }

    double weatherGrip = 1.0;
    if (weather == WeatherType::Rainy) weatherGrip = 0.85;
    else if (weather == WeatherType::Storm) weatherGrip = 0.70;

    double limitCurrent = calculateTargetSpeed(*currentSeg, weatherGrip);
    double limitNext = nextSeg ? calculateTargetSpeed(*nextSeg, weatherGrip) : limitCurrent;

    double targetSpeed = limitCurrent;

    double distToEndOfSeg = (lapDistance >= 0) ? (distanceSearch - lapDistance) : (track.segments[0].length - lapDistance);
    if (lapDistance < 0) {
        distToEndOfSeg = track.segments[0].length + std::abs(lapDistance);
    }
    
    double requiredBrakingDist = (pow(currentSpeed, 2) - pow(limitNext, 2)) / (2 * getEffectiveBraking(weatherGrip));
    
    if (currentSpeed > limitNext && distToEndOfSeg <= requiredBrakingDist * 1.1) {
        targetSpeed = limitNext;
    }

    if (carAhead && !mistakeMade) {
        double distToAhead = carAhead->getTotalDistance() - totalDistance;
        
        if (distToAhead > 0 && distToAhead < 25.0) {
            if (currentSeg->type == "CORNER") {
                targetSpeed *= 0.95; 
            }

            if (currentSpeed > carAhead->getCurrentSpeed()) {
                double paceDiff = (driver.pace - carAhead->getDriver().pace) / 100.0;
                double skillDiff = (driver.racecraft - carAhead->getDriver().racecraft) / 100.0;
                
                double overtakeChance = 0.1 + paceDiff + skillDiff; 
                if (currentSeg->type == "STRAIGHT") overtakeChance += 0.3;

                if (dist01(rng) > overtakeChance) {
                    targetSpeed = std::min(targetSpeed, carAhead->getCurrentSpeed());
                }
            }
        }
    }

    if (currentSpeed < targetSpeed) {
        currentSpeed += getEffectiveAcceleration(weatherGrip) * dt;
        if (currentSpeed > targetSpeed) currentSpeed = targetSpeed;
    } else if (currentSpeed > targetSpeed) {
        currentSpeed -= getEffectiveBraking(weatherGrip) * dt;
        if (currentSpeed < targetSpeed) currentSpeed = targetSpeed;
    }

    double moveDist = currentSpeed * dt;
    lapDistance += moveDist;
    totalDistance += moveDist;

    double wearFactor = (currentSeg->type == "CORNER") ? 2.0 : 0.5;
    double driverFactor = 1.0 + ((100 - driver.awareness) / 200.0); 
    
    // Tire wear calculation with specific tire modifier
    tireHealth -= (0.0003 * wearFactor * driverFactor * getTireWearMod() * dt); 
    if (tireHealth < 0.2) tireHealth = 0.2; 

    if (lapDistance >= track.totalLength) {
        lapDistance -= track.totalLength;
        currentLap++;
        
        // Pit logic
        if (tireHealth < 0.40 && !finished) {
            chooseNextTire(totalLaps); // AI decision
            
            std::normal_distribution<> pitDist(22.0 * team.pitStopMultiplier, 1.5);
            double pitTime = pitDist(rng);
            if (pitTime < 15.0) pitTime = 15.0;
            
            pitStopTimer = pitTime;
        }
    }
}

double Car::calculateTargetSpeed(const TrackSegment& segment, double gripModifier) {
    if (segment.type == "STRAIGHT") {
        return (team.topSpeed / 100.0) * 94.0;
    } else {
        double radius = std::abs(segment.radius);
        if (radius < 1.0) radius = 1.0;
        
        double driverSkill = 0.8 + (driver.racecraft / 500.0);
        double tireState = 0.5 + (tireHealth * 0.5);
        // Include tire specific grip
        double effectiveGrip = team.baseTireGrip * gripModifier * driverSkill * tireState * getTireGripMod();

        double vMax = std::sqrt(radius * 9.81 * effectiveGrip);
        if (vMax > 85.0) vMax = 85.0;
        return vMax;
    }
}

double Car::getEffectiveAcceleration(double gripModifier) {
    return team.acceleration * tireHealth * gripModifier * getTireGripMod();
}

double Car::getEffectiveBraking(double gripModifier) {
    return team.braking * tireHealth * gripModifier * getTireGripMod();
}

std::string Car::getStatus() const {
    std::string s;
    if (finished) s = "FINISHED";
    else if (pitStopTimer > 0) s = "IN PIT";
    else s = "Lap " + std::to_string(currentLap);
    
    s += " [" + getTireName() + "]";
    return s;
}

std::string Car::getTireName() const {
    switch(currentTire) {
        case TireCompound::Soft: return "S";
        case TireCompound::Medium: return "M";
        case TireCompound::Hard: return "H";
    }
    return "?";
}

double Car::getTireGripMod() const {
    switch(currentTire) {
        case TireCompound::Soft: return 1.15;
        case TireCompound::Medium: return 1.0;
        case TireCompound::Hard: return 0.9;
    }
    return 1.0;
}

double Car::getTireWearMod() const {
    switch(currentTire) {
        case TireCompound::Soft: return 1.5;
        case TireCompound::Medium: return 1.0;
        case TireCompound::Hard: return 0.6;
    }
    return 1.0;
}

void Car::chooseNextTire(int totalLaps) {
    int lapsRemaining = totalLaps - currentLap;
    if (lapsRemaining <= 0) {
        nextTire = TireCompound::Soft; // Doesn't matter much
        return;
    }
    
    // Simple AI Logic
    // Softs last roughly 10-15 laps (depending on wear)
    // Mediums last 20-25
    // Hards last 30+
    
    if (lapsRemaining < 15) {
        nextTire = TireCompound::Soft;
    } else if (lapsRemaining < 28) {
        nextTire = TireCompound::Medium;
    } else {
        nextTire = TireCompound::Hard;
    }
}
