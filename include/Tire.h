#ifndef PROJEKT_TIRE_H
#define PROJEKT_TIRE_H

#include <string>

enum class TireCompound {
    Soft,
    Medium,
    Hard
};

std::string tireCompoundToString(TireCompound compound);

class Tire {
public:
    Tire(TireCompound compound, double wearRate);

    TireCompound getCompound() const;
    double getGrip() const;
    double getWear() const;
    double getWearRate() const;

    void addWear(double wear);

private:
    TireCompound compound;
    double grip;
    double wear = 0.0; // 0.0 = new, 1.0 = worn out
    double wearRate;
};

#endif //PROJEKT_TIRE_H
