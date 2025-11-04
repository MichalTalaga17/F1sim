#include "Race.h"
#include "ConfigParser.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <random>
#include <iomanip>

Race::Race(Track& track, std::vector<Car*>& cars, const ConfigParser& config)
    : raceTrack(track), raceCars(cars),
      raceLaps(config.getInt("Race", "Laps")),
      rng(std::random_device{}()),
      pitStopTimeDistribution(config.getDouble("PitStop", "MinDuration"), config.getDouble("PitStop", "MaxDuration")),
      eventProbabilityDistribution(0.0, 1.0),
      carSelectionDistribution(0, raceCars.size() - 1),
      eventTypeDistribution(1, 3),
      eventDurationDistribution(config.getDouble("Events", "MinDuration"), config.getDouble("Events", "MaxDuration")),
      overtakeProbabilityDistribution(0.0, 1.0) {
    for (size_t i = 0; i < raceCars.size(); ++i) {
        raceCars[i]->setTrack(&raceTrack);
        startingPositions[raceCars[i]->getName()] = i + 1;
    }
    totalTrackLength = std::accumulate(raceTrack.getSegments().begin(), raceTrack.getSegments().end(), 0.0,
        [](double sum, const auto& segment) {
            return sum + segment->getLength();
        });
    carLaps.resize(raceCars.size(), 0);
}

void Race::run() {
    std::cout << "Race started for " << raceLaps << " laps! Total track length: " << totalTrackLength << "m" << std::endl;
    std::cout << "Weather conditions: " << weather.toString() << " (Grip: " << weather.getGripModifier() << ")" << std::endl << std::endl;

    const double deltaTime = 0.001;
    const double totalRaceDistance = totalTrackLength * raceLaps;
    size_t finishedCars = 0;

    while (finishedCars < raceCars.size()) {
        totalTime += deltaTime;

        for (size_t i = 0; i < raceCars.size(); ++i) {
            Car* car = raceCars[i];

            if (car->getIsFinished()) continue; // Skip already finished cars

            double distanceBeforeUpdate = car->getDistance();
            car->update(deltaTime, weather.getGripModifier());
            double distanceAfterUpdate = car->getDistance();

            int lapsBefore = static_cast<int>(distanceBeforeUpdate / totalTrackLength);
            int lapsAfter = static_cast<int>(distanceAfterUpdate / totalTrackLength);

            if (lapsAfter > lapsBefore) {
                carLaps[i] = lapsAfter;
                // std::cout << "[LAP] " << car->getName() << " completed lap " << lapsAfter << "!" << std::endl;

                // Check for random event
                if (eventProbabilityDistribution(rng) < 0.05) { // 5% chance per lap for an event
                    int carIndex = carSelectionDistribution(rng);
                    Car* targetCar = raceCars[carIndex];
                    CarEventType eventType = static_cast<CarEventType>(eventTypeDistribution(rng));
                    double eventDuration = eventDurationDistribution(rng);
                    targetCar->applyEvent(eventType, eventDuration);
                }

                if (car->decideToPit(lapsAfter, raceLaps)) {
                    car->pit(lapsAfter, raceLaps);
                    const double pitStopTime = pitStopTimeDistribution(rng);
                    car->addPitStopTime(pitStopTime);
                    totalTime += pitStopTime;
                }
            }

            if (car->hasFinished(totalRaceDistance) && !car->getIsFinished()) {
                car->setIsFinished(true); // Mark car as finished
                std::string strategyStr;
                const auto& tireStints = car->getTireStrategy();
                const auto& pitLaps = car->getPitLaps();
                int lastPitLap = 0;
                for (size_t j = 0; j < tireStints.size(); ++j) {
                    strategyStr += tireCompoundToString(tireStints[j])[0];
                    int endLap = (j < pitLaps.size()) ? pitLaps[j] : raceLaps;
                    strategyStr += "(" + std::to_string(endLap - lastPitLap) + ")";
                    if (j < tireStints.size() - 1) strategyStr += "-";
                    if (j < pitLaps.size()) {
                        lastPitLap = pitLaps[j];
                    }
                }

                results.push_back({car->getName(), totalTime, strategyStr, 0, car->getTotalPitStopTime()});
                finishedCars++;
            }
        }

        // Attempt overtakes after all cars have updated their positions
        // Create a temporary vector of active cars for sorting and overtaking checks
        std::vector<Car*> activeCars;
        for (Car* car : raceCars) {
            if (!car->getIsFinished()) {
                activeCars.push_back(car);
            }
        }

        std::sort(activeCars.begin(), activeCars.end(), [](const Car* a, const Car* b) {
            return a->getDistanceOnTrack() > b->getDistanceOnTrack(); // Sort in descending order (leader first)
        });

        for (size_t i = 0; i < activeCars.size(); ++i) {
            for (size_t j = i + 1; j < activeCars.size(); ++j) {
                Car* car1 = activeCars[i]; // Car ahead
                Car* car2 = activeCars[j]; // Car behind, attempting to overtake

                // Check if cars are in close proximity (e.g., within 50 meters)
                if (car1->getDistanceOnTrack() - car2->getDistanceOnTrack() < 50.0 && car1->getDistanceOnTrack() - car2->getDistanceOnTrack() > 0.0) {
                    // Car2 is attempting to overtake Car1
                    attemptOvertake(car2, car1);
                }
            }
        }
    }

    std::cout << "\nRace finished!" << std::endl;
    printResults();
}

void Race::printResults() {
    std::cout << "\n--- Race Results ---" << std::endl;
    // Sort results by time
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return a.time < b.time;
    });

    std::cout << std::left << std::setw(4) << "Pos"
              << std::left << std::setw(20) << "Driver"
              << std::left << std::setw(15) << "Time/Interval"
              << std::left << std::setw(40) << "Strategy"
              << std::left << std::setw(15) << "Pitstops (s)"
              << std::endl;
    std::cout << std::string(94, '-') << std::endl;

    if (results.empty()) {
        std::cout << "No results to display." << std::endl;
        return;
    }

    double winnerTotalTime = results[0].time;
    double winnerAverageLapTime = winnerTotalTime / raceLaps;
    double previousDriverTime = winnerTotalTime;

    for (size_t i = 0; i < results.size(); ++i) {
        auto& result = results[i];
        std::cout << std::left << std::setw(4) << (i + 1)
                  << std::left << std::setw(20) << result.carName;

        if (i == 0) {
            // First driver
            std::cout << std::left << std::setw(15) << std::fixed << std::setprecision(3) << result.time;
        } else {
            double timeDifferenceToLeader = result.time - winnerTotalTime;
            if (timeDifferenceToLeader >= winnerAverageLapTime) {
                int lapsDown = static_cast<int>(timeDifferenceToLeader / winnerAverageLapTime);
                std::cout << std::left << std::setw(15) << "+" + std::to_string(lapsDown) + " LAP(S)";
            } else {
                double timeDifferenceToPreceding = result.time - previousDriverTime;
                std::cout << std::left << std::setw(15) << "+" + std::to_string(timeDifferenceToPreceding) + "s";
            }
        }
        
        std::cout << std::left << std::setw(40) << result.strategy
                  << std::left << std::setw(15) << std::fixed << std::setprecision(3) << result.totalPitStopTime
                  << std::endl;
        previousDriverTime = result.time;
    }
    std::cout << "--------------------" << std::endl;
}

void Race::attemptOvertake(Car* attacker, Car* defender) {
    // Base probability of overtake (can be tuned)
    double overtakeChance = 0.3; 

    // Influence of attacker's Racecraft (RAC)
    overtakeChance += (attacker->getDriverRac() - 85) * 0.005; // +0.5% per point above 85

    // Influence of defender's Awareness (AWA)
    overtakeChance -= (defender->getDriverAwa() - 85) * 0.005; // -0.5% per point above 85

    // Ensure chance is within reasonable bounds
    if (overtakeChance < 0.1) overtakeChance = 0.1;
    if (overtakeChance > 0.9) overtakeChance = 0.9;

    // Check if attacker has a speed advantage (crucial for overtake)
    if (attacker->getCurrentSpeed() <= defender->getCurrentSpeed()) {
        return; // Cannot overtake if not faster
    }

    // Random decision
    if (overtakeProbabilityDistribution(rng) < overtakeChance) {
        // Overtake successful! Swap positions.
        double tempDistance = attacker->getDistanceOnTrack();
        // Set attacker's distance slightly ahead of defender
        attacker->setDistanceOnTrack(defender->getDistanceOnTrack() + 1.0); 
        defender->setDistanceOnTrack(tempDistance - 1.0); // Set defender's distance slightly behind attacker
        std::cout << "[OVERTAKE] " << attacker->getName() << " overtakes " << defender->getName() << "!" << std::endl;
    }
}