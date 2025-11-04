#include "Car.h"
#include "Agent.h"
#include "Strategy.h"
#include <iostream>
#include <cmath> // For std::sqrt and std::min

Car::Car(const Driver& driver, const Team& team, TireCompound initialTireCompound, std::unique_ptr<IStrategy> strategy,
         double softWearRate, double mediumWearRate, double hardWearRate)
    : driver(driver), team(team), agent(std::make_unique<Agent>(std::move(strategy))), rng(std::random_device{}()), performanceDistribution(-0.02, 0.02), wearDistribution(-0.1, 0.1), activeEvent({CarEventType::None, 0.0, 0.0, 0.0, 0.0}), isFinished(false),
      softWearRate(softWearRate), mediumWearRate(mediumWearRate), hardWearRate(hardWearRate),
      tire(initialTireCompound, (initialTireCompound == TireCompound::Soft ? softWearRate : (initialTireCompound == TireCompound::Medium ? mediumWearRate : hardWearRate))) {
    tireStrategy.push_back(initialTireCompound);

    // Pre-calculate effective parameters based on driver skills and team stats
    double paceFactor = 1.0 + (driver.pac - 85.0) * 0.0025;

    effectiveTopSpeed = team.topSpeed * (1.0 + (driver.pac - 85.0) * 0.001); // Pace affects top speed
    effectiveAcceleration = team.accelerationRate * paceFactor;
    effectiveBraking = team.brakingRate;
    effectiveTireGrip = team.baseTireGrip * paceFactor; // Pace affects cornering
}

void Car::setTrack(const Track* track) {
    currentTrack = track;
}

void Car::update(double deltaTime, double weatherGrip) {
    if (!currentTrack) return;

    updateEvent(deltaTime); // Update any active events

    // Determine target speed
    double targetSpeed = effectiveTopSpeed * (1.0 + performanceDistribution(rng));
    bool isCornering = false;

    double distanceIntoSegment;
    const TrackSegment* currentSegment = currentTrack->getSegmentAt(distanceOnTrack, distanceIntoSegment);

    if (const auto* corner = dynamic_cast<const Corner*>(currentSegment)) {
        isCornering = true;
        double radius = corner->getRadius();
        double totalGrip = effectiveTireGrip * tire.getGrip() * weatherGrip;
        double maxCornerSpeed = std::sqrt(totalGrip * 9.81 * radius);
        targetSpeed = std::min(effectiveTopSpeed, maxCornerSpeed);
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

    // Update tire wear
    double wearAmount = (currentSpeed / effectiveTopSpeed) * 0.01;
    if (isCornering) {
        wearAmount *= 1.5; // More wear in corners
    }
    wearAmount *= (1.0 + wearDistribution(rng)); // Apply random wear fluctuation
    tire.addWear(wearAmount * tire.getWearRate() * deltaTime);
}

double Car::getDistance() const {
    return distanceOnTrack;
}

double Car::getCurrentSpeed() const {
    return currentSpeed;
}

double Car::getDistanceOnTrack() const {
    return distanceOnTrack;
}

void Car::setDistanceOnTrack(double distance) {
    distanceOnTrack = distance;
}

const std::string& Car::getName() const {
    return driver.name;
}

bool Car::hasFinished(double trackLength) const {
    return distanceOnTrack >= trackLength;
}

bool Car::decideToPit(int currentLap, int raceLaps) {
    return agent->decideToPit(*this, currentLap, raceLaps);
}

void Car::pit(int currentLap, int raceLaps) {
    std::cout << "[PIT] " << driver.name << " is pitting!" << std::endl;
    TireCompound newTireCompound = agent->getNextTire(*this, currentLap, raceLaps);

    double currentWearRate;
    if (newTireCompound == TireCompound::Soft) {
        currentWearRate = softWearRate;
    } else if (newTireCompound == TireCompound::Medium) {
        currentWearRate = mediumWearRate;
    } else {
        currentWearRate = hardWearRate;
    }
    tire = Tire(newTireCompound, currentWearRate);
    tireStrategy.push_back(newTireCompound);
    pitLaps.push_back(currentLap);
    std::cout << "[PIT] " << driver.name << " changed to " << tireCompoundToString(newTireCompound) << " tires!" << std::endl;
}

const Tire& Car::getTire() const {
    return tire;
}

const std::vector<TireCompound>& Car::getTireStrategy() const {
    return tireStrategy;
}

const std::vector<int>& Car::getPitLaps() const {
    return pitLaps;
}

double Car::getTotalPitStopTime() const {
    return totalPitStopTime;
}

void Car::addPitStopTime(double time) {
    totalPitStopTime += time;
}

bool Car::getIsFinished() const {
    return isFinished;
}

void Car::setIsFinished(bool finished) {
    isFinished = finished;
}

int Car::getDriverRac() const {
    return driver.rac;
}

int Car::getDriverAwa() const {
    return driver.awa;
}

void Car::setStrategy(std::unique_ptr<IStrategy> strategy) {
    agent->setStrategy(std::move(strategy));
}

void Car::applyEvent(CarEventType type, double duration) {
    if (activeEvent.type != CarEventType::None) {
        // Event already active, ignore new one or queue it
        return;
    }

    activeEvent.type = type;
    activeEvent.duration = duration;

    // Store original values
    activeEvent.originalTopSpeed = effectiveTopSpeed;
    activeEvent.originalAcceleration = effectiveAcceleration;
    activeEvent.originalTireGrip = effectiveTireGrip;

    switch (type) {
        case CarEventType::MinorCarGlitch:
            effectiveTopSpeed *= 0.95; // 5% reduction
            std::cout << "[EVENT] " << driver.name << " experiences a minor car glitch for " << duration << "s!" << std::endl;
            break;
        case CarEventType::DriverError:
            effectiveAcceleration *= 0.9; // 10% reduction
            effectiveTireGrip *= 0.9; // 10% reduction
            std::cout << "[EVENT] " << driver.name << " makes a driver error for " << duration << "s!" << std::endl;
            break;
        case CarEventType::LuckyBreak:
            effectiveTopSpeed *= 1.02; // 2% boost
            std::cout << "[EVENT] " << driver.name << " gets a lucky break for " << duration << "s!" << std::endl;
            break;
        case CarEventType::None:
            break;
    }
}

void Car::updateEvent(double deltaTime) {
    if (activeEvent.type != CarEventType::None) {
        activeEvent.duration -= deltaTime;
        if (activeEvent.duration <= 0) {
            // Event ended, restore original values
            effectiveTopSpeed = activeEvent.originalTopSpeed;
            effectiveAcceleration = activeEvent.originalAcceleration;
            effectiveTireGrip = activeEvent.originalTireGrip;
            activeEvent.type = CarEventType::None;
            std::cout << "[EVENT] " << driver.name << " event ended." << std::endl;
        }
    }
}