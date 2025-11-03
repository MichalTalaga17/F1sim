#include "Weather.h"
#include <random>

Weather::Weather() {
    // Randomly determine the weather condition
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 2);
    condition = static_cast<WeatherCondition>(distr(gen));

    switch (condition) {
        case WeatherCondition::SUNNY:
            gripModifier = 1.0;
            break;
        case WeatherCondition::CLOUDY:
            gripModifier = 0.95;
            break;
        case WeatherCondition::RAINY:
            gripModifier = 0.75;
            break;
    }
}

WeatherCondition Weather::getCondition() const {
    return condition;
}

double Weather::getGripModifier() const {
    return gripModifier;
}

std::string Weather::toString() const {
    switch (condition) {
        case WeatherCondition::SUNNY:
            return "Sunny";
        case WeatherCondition::CLOUDY:
            return "Cloudy";
        case WeatherCondition::RAINY:
            return "Rainy";
    }
    return "Unknown";
}
