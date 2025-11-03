#include "Car.h"

Car::Car() = default;

void Car::setTrack(const Track* track) {
    currentTrack = track;
}

void Car::update(double deltaTime) {
    if (currentTrack) {
        distanceOnTrack += speed * deltaTime;
    }
}

double Car::getDistance() const {
    return distanceOnTrack;
}
