#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <memory>
#include "Track.h"
#include "Car.h"
#include "Race.h"

// A helper macro to turn the preprocessor definition into a string literal
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

int main() {
    std::string projectPath = TOSTRING(PROJECT_SOURCE_DIR);
    std::string trackFilePath = projectPath + "/config/oval_track.txt";

    Track track;
    if (!track.loadFromFile(trackFilePath)) {
        std::cerr << "Failed to load track file at: " << trackFilePath << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<Car>> car_storage;
    std::vector<Car*> cars;

    std::vector<std::string> drivers = {
        // Ferrari
        "Leclerc", "Hamilton",
        // McLaren
        "Norris", "Piastri",
        // Mercedes
        "Russell", "Antonelli",
        // Aston Martin
        "Alonso", "Stroll",
        // Red Bull
        "Verstappen", "Tsunoda",
        // RB
        "Lawson", "Hadjar",
        // Haas
        "Bearman", "Ocon",
        // Alpine
        "Gasly", "Colapinto",
        // Williams
        "Albon", "Sainz",
        // Kick Sauber
        "Hulkenberg", "Bortoleto"
    };

    // Setup random number generator for speeds
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distr(68.0, 70); // Speed range in m/s

    for (const auto& driver_name : drivers) {
        car_storage.push_back(std::make_unique<Car>(driver_name, distr(gen)));
        cars.push_back(car_storage.back().get());
    }

    Race race(track, cars);
    race.run();

    return 0;
}
