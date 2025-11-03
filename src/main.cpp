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

    // Define the 10 teams with slightly different performance stats
    std::map<std::string, Team> teams = {
        {"Ferrari",    {"Ferrari", 95.0, 10.5, 25.0, 1.95}},
        {"McLaren",    {"McLaren", 94.8, 10.4, 25.2, 1.92}},
        {"Mercedes",   {"Mercedes", 94.5, 10.6, 24.8, 1.93}},
        {"Aston Martin",{"Aston Martin", 94.0, 10.3, 24.5, 1.88}},
        {"Red Bull",   {"Red Bull", 95.5, 10.8, 25.5, 2.00}},
        {"RB",         {"RB", 93.5, 10.2, 24.0, 1.85}},
        {"Haas",       {"Haas", 93.0, 10.0, 23.8, 1.82}},
        {"Alpine",     {"Alpine", 93.8, 10.4, 24.2, 1.87}},
        {"Williams",   {"Williams", 93.2, 10.1, 24.1, 1.84}},
        {"Kick Sauber",{"Kick Sauber", 93.6, 10.3, 24.3, 1.86}}
    };

    // Define the 20 drivers with unique skills based on EA F1 24 ratings
    std::map<std::string, Driver> drivers = {
        // Driver Name, EXP, RAC, AWA, PAC
        {"Verstappen", {"Verstappen", 87, 94, 78, 96}},
        {"Piastri",    {"Piastri",    75, 95, 84, 94}},
        {"Norris",     {"Norris",     82, 92, 80, 94}},
        {"Leclerc",    {"Leclerc",    82, 91, 90, 93}},
        {"Russell",    {"Russell",    82, 93, 90, 92}},
        {"Hamilton",   {"Hamilton",   98, 92, 94, 88}},
        {"Alonso",     {"Alonso",     99, 88, 86, 88}},
        {"Albon",      {"Albon",      83, 88, 80, 84}},
        {"Hulkenberg", {"Hulkenberg", 88, 88, 90, 82}},
        {"Gasly",      {"Gasly",      83, 86, 82, 84}},
        {"Sainz",      {"Sainz",      87, 87, 81, 83}},
        {"Ocon",       {"Ocon",       82, 83, 85, 82}},
        {"Hadjar",     {"Hadjar",     68, 81, 80, 82}},
        {"Bortoleto",  {"Bortoleto",  66, 78, 79, 80}},
        {"Bearman",    {"Bearman",    69, 80, 70, 77}},
        {"Antonelli",  {"Antonelli",  67, 79, 73, 78}},
        {"Stroll",     {"Stroll",     83, 78, 76, 75}},
        {"Tsunoda",    {"Tsunoda",    78, 75, 73, 77}},
        {"Lawson",     {"Lawson",     70, 80, 71, 75}},
        {"Colapinto",     {"Colapinto",     58, 67, 66, 69}}
    };

    // Map drivers to their teams
    std::map<std::string, std::string> driverToTeam = {
        {"Leclerc", "Ferrari"}, {"Hamilton", "Ferrari"},
        {"Norris", "McLaren"}, {"Piastri", "McLaren"},
        {"Russell", "Mercedes"}, {"Antonelli", "Mercedes"},
        {"Alonso", "Aston Martin"}, {"Stroll", "Aston Martin"},
        {"Verstappen", "Red Bull"}, {"Lawson", "Red Bull"},
        {"Tsunoda", "RB"}, {"Hadjar", "RB"},
        {"Bearman", "Haas"}, {"Ocon", "Haas"},
        {"Gasly", "Alpine"}, {"Colapinto", "Alpine"},
        {"Albon", "Williams"}, {"Sainz", "Williams"},
        {"Hulkenberg", "Kick Sauber"}, {"Bortoleto", "Kick Sauber"}
    };

    std::vector<std::unique_ptr<Car>> car_storage;
    std::vector<Car*> cars;
    const double initialFuel = 110.0; // kg

    for (const auto& pair : driverToTeam) {
        const std::string& driverName = pair.first;
        const std::string& teamName = pair.second;
        car_storage.push_back(std::make_unique<Car>(drivers.at(driverName), teams.at(teamName), initialFuel));
        cars.push_back(car_storage.back().get());
    }

    Race race(track, cars);
    race.run();

    return 0;
}
