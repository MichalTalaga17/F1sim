#include "Tire.h"

std::string tireCompoundToString(TireCompound compound) {
    switch (compound) {
        case TireCompound::Soft:   return "Soft";
        case TireCompound::Medium: return "Medium";
        case TireCompound::Hard:   return "Hard";
    }
    return "Unknown";
}

Tire::Tire(TireCompound compound, double wearRate) : compound(compound), wearRate(wearRate) {
    switch (compound) {
        case TireCompound::Soft:
            grip = 1.1;
            break;
        case TireCompound::Medium:
            grip = 1.0;
            break;
        case TireCompound::Hard:
            grip = 0.9;
            break;
    }
}

TireCompound Tire::getCompound() const {
    return compound;
}

double Tire::getGrip() const {
    return grip * (1.0 - wear);
}

double Tire::getWear() const {
    return wear;
}

double Tire::getWearRate() const {
    return wearRate;
}

void Tire::addWear(double wear) {
    this->wear += wear;
    if (this->wear > 1.0) {
        this->wear = 1.0;
    }
}
