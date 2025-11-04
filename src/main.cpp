#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Track.h"
#include "Car.h"
#include "Race.h"
#include "Weather.h"
#include "Team.h"
#include "Driver.h"
#include "Strategy.h"
#include "ConfigParser.h"

// A helper macro to turn the preprocessor definition into a string literal
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

int main() {
    ConfigParser config;
    std::string projectPath = TOSTRING(PROJECT_SOURCE_DIR);
    std::string configFilePath = projectPath + "/config/config.txt";
    if (!config.loadFromFile(configFilePath)) {
        std::cerr << "Failed to load configuration file at: " << configFilePath << std::endl;
        return 1;
    }

    Track track;
    if (!track.loadFromFile(projectPath + "/" + config.getString("Track", "FilePath"))) {
        std::cerr << "Failed to load track file at: " << projectPath + "/" + config.getString("Track", "FilePath") << std::endl;
        return 1;
    }

    const std::map<std::string, Team>& teams = config.getTeams();
    const std::map<std::string, Driver>& drivers = config.getDrivers();
    const std::map<std::string, std::string>& driverToTeam = config.getDriverToTeamMap();

    std::vector<std::unique_ptr<Car>> car_storage;
    std::vector<Car*> cars;

    auto getStrategy = [&](int driver_i) -> std::unique_ptr<IStrategy> {
        if (driver_i < 6) { // Top drivers
            return std::make_unique<AggressiveStrategy>(config.getDouble("Strategies", "AggressivePitThreshold"));
        } else if (driver_i < 14) { // Mid-field drivers
            return std::make_unique<BalancedStrategy>(config.getDouble("Strategies", "BalancedPitThreshold"));
        } else { // Backmarkers
            return std::make_unique<ConservativeStrategy>(config.getDouble("Strategies", "ConservativePitThreshold"));
        }
    };

    int driver_i = 0;
    for (const auto& pair : driverToTeam) {
        const std::string& driverName = pair.first;
        const std::string& teamName = pair.second;
        TireCompound startCompound = (driver_i < 10) ? TireCompound::Soft : TireCompound::Medium;
        car_storage.push_back(std::make_unique<Car>(drivers.at(driverName), teams.at(teamName), startCompound, getStrategy(driver_i),
                                                     config.getDouble("TireWear", "SoftWearRate"),
                                                     config.getDouble("TireWear", "MediumWearRate"),
                                                     config.getDouble("TireWear", "HardWearRate")));
        cars.push_back(car_storage.back().get());
        driver_i++;
    }

    std::cout << "--- Starting Grid & Tires ---" << std::endl;
    for (const auto* car : cars) {
        std::cout << car->getName() << " on " << tireCompoundToString(car->getTire().getCompound()) << " tires" << std::endl;
    }
    std::cout << "---------------------------" << std::endl << std::endl;

    Race race(track, cars, config);
    race.run();

    return 0;
}
