#include "../include/Car.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Car::Car(const Driver& d, const Team& t, int startGridPosition) : driver(d), team(t) {
    totalDistance = -static_cast<double>(startGridPosition) * 8.0;
    lapDistance = totalDistance;
    strategyHistory.push_back(currentCompound);
}

void Car::update(double dt, const Track& track, int totalLaps, WeatherType weather, std::mt19937& rng, const Car* carAhead) {
    if (finished) return;

    if (pitStopTimer > 0) {
        pitStopTimer -= dt;
        if (pitStopTimer <= 0) {
            tireHealth = 1.0;
            lapStartTireHealth = 1.0;
            pitStopTimer = 0;
            
            // Change tires
            std::vector<std::string> compounds = {"S", "M", "H"};
            std::uniform_int_distribution<> dist(0, 2);
            currentCompound = compounds[dist(rng)];
            strategyHistory.push_back(currentCompound);
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
    tireHealth -= (0.0003 * wearFactor * driverFactor * dt); 
    if (tireHealth < 0.2) tireHealth = 0.2; 

    if (lapDistance >= track.totalLength) {
        // Track tire wear
        double currentWear = lapStartTireHealth - tireHealth;
        if (currentWear > 0) {
            tireWearPerLap = 0.7 * tireWearPerLap + 0.3 * currentWear;
        }
        lapStartTireHealth = tireHealth;

        lapDistance -= track.totalLength;
        currentLap++;
        
        if (!finished) {
            // Strategia "What-If"
            int lapsRemaining = totalLaps - (currentLap - 1);
            
            // Only consider pitting if we have reasonable wear or strategic need
            if (lapsRemaining > 0) {
                std::normal_distribution<> pitDist(22.0 * team.pitStopMultiplier, 1.5);
                double estimatedPitTime = pitDist(rng); 
                if (estimatedPitTime < 15.0) estimatedPitTime = 15.0;

                // Simple base lap time estimate (avg speed ~60m/s)
                double baseLapTime = track.totalLength / 60.0; 

                // Simulation Lambda
                auto simulateTime = [&](double startHealth, bool pitNow) -> double {
                    double totalTime = 0.0;
                    double simHealth = startHealth;
                    
                    if (pitNow) {
                        totalTime += estimatedPitTime;
                        simHealth = 1.0;
                    }

                    for (int i = 0; i < lapsRemaining; ++i) {
                        // Forced pit logic in simulation
                        if (simHealth < 0.25) { // Critical threshold
                            totalTime += estimatedPitTime;
                            simHealth = 1.0;
                        }
                        
                        double tireState = 0.5 + (simHealth * 0.5);
                        // Speed factor approx sqrt(tireState)
                        // Time factor approx 1/sqrt(tireState)
                        double speedFactor = std::sqrt(tireState);
                        double lapTime = baseLapTime / speedFactor;
                        
                        totalTime += lapTime;
                        simHealth -= tireWearPerLap;
                        if (simHealth < 0.2) simHealth = 0.2;
                    }
                    return totalTime;
                };

                double timeKeepGoing = simulateTime(tireHealth, false);
                double timePitNow = simulateTime(tireHealth, true);

                // Decision
                // If pitting now is faster by a margin (e.g. 1.0s), do it.
                // Also force pit if tire is dangerous (< 20%)
                if (tireHealth < 0.25 || timePitNow < (timeKeepGoing - 1.0)) {
                    pitStopTimer = estimatedPitTime;
                }
            }
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
        double effectiveGrip = team.baseTireGrip * gripModifier * driverSkill * tireState;

        double vMax = std::sqrt(radius * 9.81 * effectiveGrip);
        if (vMax > 85.0) vMax = 85.0;
        return vMax;
    }
}

double Car::getEffectiveAcceleration(double gripModifier) {
    return team.acceleration * tireHealth * gripModifier;
}

double Car::getEffectiveBraking(double gripModifier) {
    return team.braking * tireHealth * gripModifier;
}

std::string Car::getStatus() const {
    if (finished) return "FINISHED";
    if (pitStopTimer > 0) return "IN PIT";
    return "Lap " + std::to_string(currentLap);
}

std::string Car::getStrategyString() const {
    std::string s = "";
    for (size_t i = 0; i < strategyHistory.size(); ++i) {
        s += "[" + strategyHistory[i] + "]";
        if (i < strategyHistory.size() - 1) {
            s += " -> ";
        }
    }
    return s;
}
