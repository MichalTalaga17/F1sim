#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <random>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "../include/ConfigParser.h"
#include "../include/Race.h"
#include "../include/Car.h"

namespace fs = std::filesystem;

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printHeader(const std::string& title) {
    std::cout << "\n========================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "========================================\n";
}

struct GridPosition {
    int position;
    Driver driver;
    Team team;
    double qualifyingTime;
};

std::vector<GridPosition> simulateQualifying(const std::vector<Driver>& drivers, const std::vector<Team>& teams, const Track& track) {
    std::vector<GridPosition> grid;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0, 0.2);

    std::map<std::string, Team> teamMap;
    for (const auto& t : teams) teamMap[t.name] = t;

    double baseLapTime = 0.0;
    
    for (const auto& seg : track.segments) {
        double segmentSpeed = 0;
        
        if (seg.type == "STRAIGHT") {
            if (seg.length > 500) segmentSpeed = 75.0;
            else segmentSpeed = 55.0;
        } else {
            double absRadius = std::abs(seg.radius);
            if (absRadius < 1.0) absRadius = 1.0;

            double cornerGrip = 2.5;
            double vMax = std::sqrt(absRadius * 9.81 * cornerGrip);
            
            if (vMax > 85.0) vMax = 85.0; 
            segmentSpeed = vMax;
        }
        
        if (segmentSpeed < 1.0) segmentSpeed = 1.0;
        baseLapTime += (seg.length / segmentSpeed);
    }
    
    baseLapTime *= 1.10; 

    for (const auto& driver : drivers) {
        Team car = teamMap[driver.teamName];
        
        double carScore = (car.topSpeed / 100.0) * 0.4 + (car.acceleration / 15.0) * 0.3 + (car.baseTireGrip / 2.5) * 0.3;
        double driverScore = (driver.pace / 100.0) * 0.7 + (driver.racecraft / 100.0) * 0.3;
        double totalPerformance = (carScore * 0.65 + driverScore * 0.35);
        double timeMultiplier = 1.45 - (totalPerformance * 0.6); 

        double simulatedTime = baseLapTime * timeMultiplier + d(gen);

        GridPosition pos;
        pos.driver = driver;
        pos.team = car;
        pos.qualifyingTime = simulatedTime;
        grid.push_back(pos);
    }

    std::sort(grid.begin(), grid.end(), [](const GridPosition& a, const GridPosition& b) {
        return a.qualifyingTime < b.qualifyingTime;
    });

    for (size_t i = 0; i < grid.size(); ++i) {
        grid[i].position = i + 1;
    }

    return grid;
}

Track selectTrack() {
    std::vector<std::string> trackFiles;
    std::string configDir = "config";

    if (!fs::exists(configDir)) {
        if (fs::exists("../config")) configDir = "../config";
        else if (fs::exists("../../config")) configDir = "../../config";
    }
    
    for (const auto& entry : fs::directory_iterator(configDir)) {
        if (entry.path().extension() == ".txt" && entry.path().filename() != "config.txt") {
            trackFiles.push_back(entry.path().string());
        }
    }

    printHeader("SELECT TRACK");
    for (size_t i = 0; i < trackFiles.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << fs::path(trackFiles[i]).stem().string() << "\n";
    }

    int choice;
    std::cout << "\nChoice: ";
    std::cin >> choice;

    if (choice < 1 || choice > trackFiles.size()) choice = 1;

    ConfigParser parser;
    return parser.loadTrack(trackFiles[choice - 1]);
}

WeatherType selectWeather() {
    printHeader("SELECT WEATHER CONDITIONS");
    std::cout << "[1] Sunny (Optimal Grip)\n";
    std::cout << "[2] Cloudy (Cooler Track)\n";
    std::cout << "[3] Rainy (Low Grip - Intermediates)\n";
    std::cout << "[4] Storm (Extreme - Full Wets)\n";

    int choice;
    std::cout << "\nChoice: ";
    std::cin >> choice;

    switch (choice) {
        case 1: return WeatherType::Sunny;
        case 2: return WeatherType::Cloudy;
        case 3: return WeatherType::Rainy;
        case 4: return WeatherType::Storm;
        default: return WeatherType::Sunny;
    }
}

std::string formatTime(double seconds) {
    int minutes = static_cast<int>(seconds) / 60;
    double remainingSeconds = seconds - (minutes * 60);
    std::stringstream ss;
    ss << minutes << ":" << std::fixed << std::setprecision(3) << (remainingSeconds < 10 ? "0" : "") << remainingSeconds;
    return ss.str();
}

int main() {
    ConfigParser parser;
    std::string configPath = "config/config.txt";
    
    if (!fs::exists(configPath)) {
        if (fs::exists("../config/config.txt")) {
            configPath = "../config/config.txt";
        } else if (fs::exists("../../config/config.txt")) {
            configPath = "../../config/config.txt";
        }
    }

    if (!parser.loadMainConfig(configPath)) {
        std::cerr << "CRITICAL ERROR: Cannot open config file. Searching in: " << fs::current_path() << std::endl;
        return 1;
    }

    clearConsole();
    std::cout << "############################################\n";
    std::cout << "#      F1 STRATEGY SIMULATOR 2025          #\n";
    std::cout << "############################################\n";

    Track selectedTrack = selectTrack();
    std::cout << "Selected Track: " << selectedTrack.name << " (" << (int)selectedTrack.totalLength << "m)\n";

    WeatherType weather = selectWeather();

    auto drivers = parser.getDrivers();
    auto teams = parser.getTeams();

    std::cout << "\nGenerating Starting Grid based on Q3 Simulation...\n";

    auto grid = simulateQualifying(drivers, teams, selectedTrack);

    clearConsole();
    printHeader("STARTING GRID - " + selectedTrack.name);
    std::cout << "\n";

    for (const auto& pos : grid) {
        std::stringstream ss;
        ss << std::left << std::setw(2) << pos.position << "." << std::setw(15) << pos.driver.name ;

        if (pos.position % 2 != 0) {
            std::cout << ss.str() << "\n\n";
        } else {
            std::cout << std::string(10, ' ') << ss.str() << "\n\n";
        }
    }

    std::cout << "\nPress ENTER to start the race...";
    std::cin.ignore(); 
    std::cin.get();

    std::vector<Car> raceCars;
    for (size_t i = 0; i < grid.size(); ++i) {
        Car car(grid[i].driver, grid[i].team, i);
        raceCars.push_back(car);
    }

    int laps = parser.getGlobalConfig().laps;
    if (laps <= 0) laps = 10;

    Race race(raceCars, selectedTrack, laps);
    race.run();

    return 0;
}
