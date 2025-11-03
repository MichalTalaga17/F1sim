#include "Race.h"
#include <iostream>
#include <numeric>

Race::Race(Track& track, Car& car) : raceTrack(track), raceCar(car) {
    raceCar.setTrack(&raceTrack);
    totalTrackLength = std::accumulate(raceTrack.getSegments().begin(), raceTrack.getSegments().end(), 0.0,
        [](double sum, const auto& segment) {
            return sum + segment->getLength();
        });
}

void Race::run() {
    std::cout << "Race started! Total track length: " << totalTrackLength << "m" << std::endl;
    const double deltaTime = 0.1; // 100ms simulation step

    while (raceCar.getDistance() < totalTrackLength) {
        raceCar.update(deltaTime);
        totalTime += deltaTime;
    }

    std::cout << "Race finished!" << std::endl;
    std::cout << "Lap time: " << totalTime << " seconds" << std::endl;
}
