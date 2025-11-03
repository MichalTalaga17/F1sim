#include "Race.h"
#include <iostream>
#include <numeric>
#include <algorithm>

Race::Race(Track& track, std::vector<Car*>& cars) : raceTrack(track), raceCars(cars) {
    for (auto* car : raceCars) {
        car->setTrack(&raceTrack);
    }
    totalTrackLength = std::accumulate(raceTrack.getSegments().begin(), raceTrack.getSegments().end(), 0.0,
        [](double sum, const auto& segment) {
            return sum + segment->getLength();
        });
}

void Race::run() {
    std::cout << "Race started! Total track length: " << totalTrackLength << "m" << std::endl;
    std::cout << "Weather conditions: " << weather.toString() << " (Grip: " << weather.getGripModifier() << ")" << std::endl << std::endl;

    const double deltaTime = 0.001;
    size_t finishedCars = 0;

    while (finishedCars < raceCars.size()) {
        totalTime += deltaTime;
        
        for (auto* car : raceCars) {
            // Only update cars that haven't finished
            if (!car->hasFinished(totalTrackLength)) {
                car->update(deltaTime, weather.getGripModifier());
                // Check if the car finished in this step
                if (car->hasFinished(totalTrackLength)) {
                    results.emplace_back(car->getName(), totalTime);
                }
            }
        }

        // Count finished cars after update
        size_t count = 0;
        for (auto* car : raceCars) {
            if (car->hasFinished(totalTrackLength)) {
                count++;
            }
        }
        finishedCars = count;
    }

    std::cout << "\nRace finished!" << std::endl;
    printResults();
}

void Race::printResults() {
    std::cout << "\n--- Race Results ---" << std::endl;
    // Sort results by time
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });

    int pos = 1;
    for (const auto& result : results) {
        std::cout << pos++ << ". " << result.first << "\tTime: " << result.second << "s" << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}
