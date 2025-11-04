#include "Strategy.h"
#include "Car.h"

// Aggressive Strategy
AggressiveStrategy::AggressiveStrategy(double pitThreshold) : pitThreshold(pitThreshold) {}
bool AggressiveStrategy::shouldPit(const Car& car, int currentLap, int raceLaps) {
    return car.getTire().getWear() > pitThreshold && (raceLaps - currentLap > 2);
}

TireCompound AggressiveStrategy::getNextTire(const Car& car, int currentLap, int raceLaps) {
    if (raceLaps - currentLap < 15) {
        return TireCompound::Soft;
    }
    return TireCompound::Medium;
}

// Balanced Strategy
BalancedStrategy::BalancedStrategy(double pitThreshold) : pitThreshold(pitThreshold) {}
bool BalancedStrategy::shouldPit(const Car& car, int currentLap, int raceLaps) {
    return car.getTire().getWear() > pitThreshold && (raceLaps - currentLap > 2);
}

TireCompound BalancedStrategy::getNextTire(const Car& car, int currentLap, int raceLaps) {
    if (raceLaps - currentLap < 10) {
        return TireCompound::Soft;
    }
    if (raceLaps - currentLap > 25) {
        return TireCompound::Hard;
    }
    return TireCompound::Medium;
}

// Conservative Strategy
ConservativeStrategy::ConservativeStrategy(double pitThreshold) : pitThreshold(pitThreshold) {}
bool ConservativeStrategy::shouldPit(const Car& car, int currentLap, int raceLaps) {
    return car.getTire().getWear() > pitThreshold && (raceLaps - currentLap > 2);
}

TireCompound ConservativeStrategy::getNextTire(const Car& car, int currentLap, int raceLaps) {
    if (raceLaps - currentLap < 20) {
        return TireCompound::Medium;
    }
    return TireCompound::Hard;
}
