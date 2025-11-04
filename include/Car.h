#ifndef PROJEKT_CAR_H
#define PROJEKT_CAR_H

#include <string>
#include "Track.h"
#include "Team.h"
#include "Driver.h"
#include "Agent.h"
#include "Tire.h"
#include <memory>
#include <random>
#include <map>

enum class CarEventType {
    None,
    MinorCarGlitch,
    DriverError,
    LuckyBreak
};

struct ActiveCarEvent {
    CarEventType type;
    double duration;
    double originalTopSpeed;
    double originalAcceleration;
    double originalTireGrip;
};

class Car {
public:
    explicit Car(const Driver& driver, const Team& team, TireCompound initialTireCompound, std::unique_ptr<IStrategy> strategy,
                 double softWearRate, double mediumWearRate, double hardWearRate);
    void update(double deltaTime, double weatherGrip);
    double getDistance() const;
    double getCurrentSpeed() const;
    double getDistanceOnTrack() const;
    void setDistanceOnTrack(double distance);
    void setTrack(const Track* track);
    const std::string& getName() const;
    bool hasFinished(double trackLength) const;
    bool decideToPit(int currentLap, int raceLaps);
    void pit(int currentLap, int raceLaps);
    const Tire& getTire() const;
    const std::vector<TireCompound>& getTireStrategy() const;
    const std::vector<int>& getPitLaps() const;
    double getTotalPitStopTime() const;
    bool getIsFinished() const;
    void setIsFinished(bool finished);
    int getDriverRac() const;
    int getDriverAwa() const;
    void addPitStopTime(double time);
    void setStrategy(std::unique_ptr<IStrategy> strategy);

    void applyEvent(CarEventType type, double duration);

private:
    // Identity
    const Driver& driver;
    const Team& team;
    std::unique_ptr<Agent> agent;

    // Components
    Tire tire;
    std::vector<TireCompound> tireStrategy;

    // Pre-calculated effective parameters
    double effectiveTopSpeed;
    double effectiveAcceleration;
    double effectiveBraking;
    double effectiveTireGrip;

    // Physics & State
    const Track* currentTrack = nullptr;
    double distanceOnTrack = 0.0;
    double currentSpeed = 0.0; // m/s
    bool isFinished = false;

    // Parameters
    double baseMass = 798.0; // kg
    std::vector<int> pitLaps;
    double totalPitStopTime = 0.0;

    double softWearRate;
    double mediumWearRate;
    double hardWearRate;

    std::mt19937 rng;
    std::uniform_real_distribution<double> performanceDistribution;
    std::uniform_real_distribution<double> wearDistribution;

    ActiveCarEvent activeEvent;
    void updateEvent(double deltaTime);
};

#endif //PROJEKT_CAR_H
