#include "../include/Car.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <map>

// Helper to get tire stats
static void getTireStats(const std::string& compound, double& wear, double& grip) {
    if (compound == "S") {
        wear = 0.04;
        grip = 1.10;
    } else if (compound == "H") {
        wear = 0.01;
        grip = 0.90;
    } else { // Medium
        wear = 0.02;
        grip = 1.00;
    }
}

Car::Car(const Driver& d, const Team& t, int startGridPosition) : driver(d), team(t) {
    totalDistance = -static_cast<double>(startGridPosition) * 8.0;
    lapDistance = totalDistance;
    
    // Randomize Start Compound
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<std::string> compounds = {"S", "M", "H"};
    std::uniform_int_distribution<> dist(0, 2);
    currentCompound = compounds[dist(gen)];
    
    strategyHistory.push_back({currentCompound, 0});
    
    double gripMod = 1.0;
    getTireStats(currentCompound, tireWearPerLap, gripMod);
}

void Car::update(double dt, const Track& track, int totalLaps, WeatherType weather, std::mt19937& rng, const Car* carAhead) {
    if (finished) return;

    if (pitStopTimer > 0) {
        pitStopTimer -= dt;
        if (pitStopTimer <= 0) {
            tireHealth = 1.0;
            lapStartTireHealth = 1.0;
            pitStopTimer = 0;
            
            // Choose new tire based on remaining laps (Simple Logic)
            int lapsRemaining = totalLaps - currentLap;
            if (lapsRemaining <= 15) {
                currentCompound = "S";
            } else if (lapsRemaining <= 25) {
                currentCompound = "M";
            } else {
                currentCompound = "H";
            }
            
            // Random chance to mix it up slightly (to keep some variety)
            std::uniform_real_distribution<> d(0.0, 1.0);
            if (d(rng) < 0.2) {
                 std::vector<std::string> opts = {"S", "M", "H"};
                 std::uniform_int_distribution<> dist(0, 2);
                 currentCompound = opts[dist(rng)];
            }

            strategyHistory.push_back({currentCompound, 0});
            
            double gripMod = 1.0;
            getTireStats(currentCompound, tireWearPerLap, gripMod);
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
    
    // Use compound specific wear
    tireHealth -= (0.0003 * wearFactor * driverFactor * dt * (tireWearPerLap / 0.02)); 
    if (tireHealth < 0.2) tireHealth = 0.2; 

    if (lapDistance >= track.totalLength) {
        // Track tire wear
        double currentWear = lapStartTireHealth - tireHealth;
        if (currentWear > 0) {
            // Adapt observed wear
            // tireWearPerLap = 0.7 * tireWearPerLap + 0.3 * currentWear; 
            // Better to stick to compound property but maybe slight adaptation?
            // Let's keep it simple for now and trust the compound stats.
        }
        lapStartTireHealth = tireHealth;

        lapDistance -= track.totalLength;
        currentLap++;
        
        // Increment lap count for current stint
        if (!strategyHistory.empty()) {
            strategyHistory.back().second++;
        }
        
        if (!finished) {
            // Strategia "What-If"
            int lapsRemaining = totalLaps - (currentLap - 1);
            
            // Only consider pitting if we have reasonable wear or strategic need
            if (lapsRemaining > 0) {
                std::normal_distribution<> pitDist(22.0 * team.pitStopMultiplier, 1.5);
                double estimatedPitTime = pitDist(rng); 
                if (estimatedPitTime < 15.0) estimatedPitTime = 15.0;

                // Base lap time (approximate)
                double baseLapTime = track.totalLength / 60.0; 

                // Determine what tire we WOULD get if we pit now
                std::string nextTire = "M";
                if (lapsRemaining <= 15) nextTire = "S";
                else if (lapsRemaining > 25) nextTire = "H";
                
                double nextWear, nextGrip;
                getTireStats(nextTire, nextWear, nextGrip);
                
                // Simulation Lambda
                auto simulateTime = [&](double startHealth, double currentWearRate, double currentGrip, bool pitNow) -> double {
                    double totalTime = 0.0;
                    double simHealth = startHealth;
                    double simWear = currentWearRate;
                    double simGrip = currentGrip;
                    
                    int currentSimLap = 0;

                    if (pitNow) {
                        totalTime += estimatedPitTime;
                        simHealth = 1.0;
                        simWear = nextWear;
                        simGrip = nextGrip;
                    }

                    for (int i = 0; i < lapsRemaining; ++i) {
                        // If tire dead, must pit
                        if (simHealth < 0.25) { 
                            totalTime += estimatedPitTime;
                            simHealth = 1.0;
                            // Assume we pick same optimal tire again for simplicity or standard M
                            // For simplicity, reset to decent tire stats
                            simWear = 0.02; 
                            simGrip = 1.0;
                        }
                        
                        double tireState = 0.5 + (simHealth * 0.5);
                        // Speed factor: grip * tireState
                        double speedFactor = std::sqrt(simGrip * tireState);
                        double lapTime = baseLapTime / speedFactor;
                        
                        totalTime += lapTime;
                        simHealth -= simWear; // Simple per-lap wear
                        if (simHealth < 0.2) simHealth = 0.2;
                    }
                    return totalTime;
                };
                
                double currentGripMod = 1.0;
                double dummyWear;
                getTireStats(currentCompound, dummyWear, currentGripMod);

                double timeKeepGoing = simulateTime(tireHealth, tireWearPerLap, currentGripMod, false);
                double timePitNow = simulateTime(tireHealth, tireWearPerLap, currentGripMod, true);

                // Decision
                // Pit if time saved > 1.0s or tire critical
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
        
        // Tire Grip Influence
        double tireGripBase = 1.0;
        double dummy;
        getTireStats(currentCompound, dummy, tireGripBase);
        
        double tireState = 0.5 + (tireHealth * 0.5);
        double effectiveGrip = team.baseTireGrip * gripModifier * driverSkill * tireState * tireGripBase;

        double vMax = std::sqrt(radius * 9.81 * effectiveGrip);
        if (vMax > 85.0) vMax = 85.0;
        return vMax;
    }
}

double Car::getEffectiveAcceleration(double gripModifier) {
    double tireGripBase = 1.0;
    double dummy;
    getTireStats(currentCompound, dummy, tireGripBase);
    return team.acceleration * tireHealth * gripModifier * tireGripBase;
}

double Car::getEffectiveBraking(double gripModifier) {
    double tireGripBase = 1.0;
    double dummy;
    getTireStats(currentCompound, dummy, tireGripBase);
    return team.braking * tireHealth * gripModifier * tireGripBase;
}

std::string Car::getStatus() const {
    if (finished) return "FINISHED";
    if (pitStopTimer > 0) return "IN PIT";
    return "Lap " + std::to_string(currentLap) + " [" + currentCompound + "]";
}

std::string Car::getStrategyString() const {
    std::string s = "";
    for (size_t i = 0; i < strategyHistory.size(); ++i) {
        s += strategyHistory[i].first + "(" + std::to_string(strategyHistory[i].second) + ")";
        if (i < strategyHistory.size() - 1) {
            s += "-";
        }
    }
    return s;
}