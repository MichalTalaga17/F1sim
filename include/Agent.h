#ifndef PROJEKT_AGENT_H
#define PROJEKT_AGENT_H

#include <memory>
#include "Strategy.h"

class Car; // Forward declaration

class Agent {
public:
    explicit Agent(std::unique_ptr<IStrategy> strategy);
    bool decideToPit(const Car& car, int currentLap, int raceLaps);
    TireCompound getNextTire(const Car& car, int currentLap, int raceLaps);
    void setStrategy(std::unique_ptr<IStrategy> strategy);

private:
    std::unique_ptr<IStrategy> currentStrategy;
};

#endif //PROJEKT_AGENT_H
