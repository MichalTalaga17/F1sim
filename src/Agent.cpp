#include "Agent.h"

Agent::Agent(std::unique_ptr<IStrategy> strategy) : currentStrategy(std::move(strategy)) {}

bool Agent::decideToPit(const Car& car, int currentLap, int raceLaps) {
    if (currentStrategy) {
        return currentStrategy->shouldPit(car, currentLap, raceLaps);
    }
    return false;
}

TireCompound Agent::getNextTire(const Car& car, int currentLap, int raceLaps) {
    if (currentStrategy) {
        return currentStrategy->getNextTire(car, currentLap, raceLaps);
    }
    return TireCompound::Medium; // Default fallback
}

void Agent::setStrategy(std::unique_ptr<IStrategy> strategy) {
    currentStrategy = std::move(strategy);
}
