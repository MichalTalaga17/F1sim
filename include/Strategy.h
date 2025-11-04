#ifndef PROJEKT_STRATEGY_H
#define PROJEKT_STRATEGY_H

#include "Tire.h"

class Car; // Forward declaration

class IStrategy {
public:
    virtual ~IStrategy() = default;
    virtual bool shouldPit(const Car& car, int currentLap, int raceLaps) = 0;
    virtual TireCompound getNextTire(const Car& car, int currentLap, int raceLaps) = 0;
};

class AggressiveStrategy : public IStrategy {
public:
    explicit AggressiveStrategy(double pitThreshold);
    bool shouldPit(const Car& car, int currentLap, int raceLaps) override;
    TireCompound getNextTire(const Car& car, int currentLap, int raceLaps) override;
private:
    double pitThreshold;
};

class BalancedStrategy : public IStrategy {
public:
    explicit BalancedStrategy(double pitThreshold);
    bool shouldPit(const Car& car, int currentLap, int raceLaps) override;
    TireCompound getNextTire(const Car& car, int currentLap, int raceLaps) override;
private:
    double pitThreshold;
};

class ConservativeStrategy : public IStrategy {
public:
    explicit ConservativeStrategy(double pitThreshold);
    bool shouldPit(const Car& car, int currentLap, int raceLaps) override;
    TireCompound getNextTire(const Car& car, int currentLap, int raceLaps) override;
private:
    double pitThreshold;
};


#endif //PROJEKT_STRATEGY_H
