#include "Car.h"

Car::Car(std::string name, double speed) : name(std::move(name)), speed(speed) {}

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

const std::string& Car::getName() const {
    return name;
}

bool Car::hasFinished(double trackLength) const {
    return distanceOnTrack >= trackLength;
}
