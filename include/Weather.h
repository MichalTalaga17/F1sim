#ifndef PROJEKT_WEATHER_H
#define PROJEKT_WEATHER_H

#include <string>

enum class WeatherCondition {
    SUNNY,
    CLOUDY,
    RAINY
};

class Weather {
public:
    Weather();
    WeatherCondition getCondition() const;
    double getGripModifier() const;
    std::string toString() const;

private:
    WeatherCondition condition;
    double gripModifier;
};

#endif //PROJEKT_WEATHER_H
