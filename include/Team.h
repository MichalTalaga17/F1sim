#ifndef PROJEKT_TEAM_H
#define PROJEKT_TEAM_H

#include <string>

struct Team {
    std::string name;
    double topSpeed = 95.0; // m/s
    double accelerationRate = 10.0; // m/s^2
    double brakingRate = 25.0; // m/s^2
    double baseTireGrip = 1.9; // Base grip coefficient
};

#endif //PROJEKT_TEAM_H
