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

// --- Helper Functions ---

void clearConsole() {
    // Prosty hack na czyszczenie konsoli
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

// --- Logic ---

struct GridPosition {
    int position;
    Driver driver;
    Team team;
    double qualifyingTime; // Simulated time
};

// Funkcja symulująca kwalifikacje
std::vector<GridPosition> simulateQualifying(const std::vector<Driver>& drivers, const std::vector<Team>& teams, const Track& track) {
    std::vector<GridPosition> grid;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0, 0.2); // Losowy czynnik +/- 0.2s

    // Mapowanie nazw zespołów na obiekty Team dla szybszego dostępu
    std::map<std::string, Team> teamMap;
    for (const auto& t : teams) teamMap[t.name] = t;

    // --- Obliczanie realistycznego czasu bazowego ---
    double baseLapTime = 0.0;
    
    // Iterujemy przez segmenty, aby oszacować czas przejazdu idealnego bolidu
    for (const auto& seg : track.segments) {
        double segmentSpeed = 0;
        
        if (seg.type == "STRAIGHT") {
            // Na prostej średnia jest wysoka, ale zależy od długości (przyspieszanie)
            // Uproszczenie: długa prosta > 280km/h (77m/s), krótka > 200km/h (55m/s)
            if (seg.length > 500) segmentSpeed = 75.0; // ~270 km/h avg
            else segmentSpeed = 55.0; // ~200 km/h avg
        } else {
            // W zakręcie prędkość zależy od promienia (fizyka: v^2/r = g*mu)
            // V_max = sqrt(Radius * 9.81 * Grip)
            // Używamy abs(), bo minus oznacza kierunek skrętu
            double absRadius = std::abs(seg.radius);
            
            // Zabezpieczenie przed zerowym promieniem (błąd w pliku)
            if (absRadius < 1.0) absRadius = 1.0;

            double cornerGrip = 2.5; // Aerodynamika + Opony
            double vMax = std::sqrt(absRadius * 9.81 * cornerGrip);
            
            // Ograniczenie górne (nie jedziemy 400km/h w łuku)
            if (vMax > 85.0) vMax = 85.0; 
            
            segmentSpeed = vMax;
        }
        
        // Zabezpieczenie przed dzieleniem przez zero (powinno być niemożliwe, ale dmuchamy na zimne)
        if (segmentSpeed < 1.0) segmentSpeed = 1.0;

        baseLapTime += (seg.length / segmentSpeed);
    }
    
    // Dodajemy mały narzut na hamowanie (czas tracony na wejściach w zakręty)
    // Zakładamy, że wyliczony powyżej czas jest "perfect flow", w realu traci się na dohamowaniach.
    baseLapTime *= 1.10; 

    for (const auto& driver : drivers) {
        Team car = teamMap[driver.teamName];
        
        // --- Algorytm "Performance Score" ---
        // Normalizacja do okolic 1.0 dla przeciętnego bolidu
        // TopSpeed ~95 (0.95), Acc ~10.5 (0.7), Grip ~1.9 (0.76)
        double carScore = (car.topSpeed / 100.0) * 0.4 + (car.acceleration / 15.0) * 0.3 + (car.baseTireGrip / 2.5) * 0.3;
        
        // Driver skill (~0.8 - 0.95)
        double driverScore = (driver.pace / 100.0) * 0.7 + (driver.racecraft / 100.0) * 0.3;

        // Combined Performance (Wartość w okolicach 0.8 - 1.1)
        // Im wyższa wartość, tym SZYBCIEJ jedziemy (mniejszy czas)
        double totalPerformance = (carScore * 0.65 + driverScore * 0.35);
        
        // Modyfikator czasu: Lepszy performance = mniejszy mnożnik czasu
        // Np. Performance 1.0 -> Mnożnik 0.85
        // Performance 0.8 -> Mnożnik 0.95
        double timeMultiplier = 1.45 - (totalPerformance * 0.6); 

        // Symulacja czasu: BaseTime * Multiplier + RandomError
        double simulatedTime = baseLapTime * timeMultiplier + d(gen);

        GridPosition pos;
        pos.driver = driver;
        pos.team = car;
        pos.qualifyingTime = simulatedTime;
        grid.push_back(pos);
    }

    // Sortowanie od najmniejszego czasu (Pole Position)
    std::sort(grid.begin(), grid.end(), [](const GridPosition& a, const GridPosition& b) {
        return a.qualifyingTime < b.qualifyingTime;
    });

    // Przypisanie pozycji
    for (size_t i = 0; i < grid.size(); ++i) {
        grid[i].position = i + 1;
    }

    return grid;
}

// --- Menus ---

Track selectTrack() {
    std::vector<std::string> trackFiles;
    std::string configDir = "config";

    if (!fs::exists(configDir)) {
        if (fs::exists("../config")) configDir = "../config";
        else if (fs::exists("../../config")) configDir = "../../config";
    }
    
    // Skanowanie katalogu
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

    if (choice < 1 || choice > trackFiles.size()) choice = 1; // Default to first

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

// Helper do formatowania czasu
std::string formatTime(double seconds) {
    int minutes = static_cast<int>(seconds) / 60;
    double remainingSeconds = seconds - (minutes * 60);
    std::stringstream ss;
    ss << minutes << ":" << std::fixed << std::setprecision(3) << (remainingSeconds < 10 ? "0" : "") << remainingSeconds;
    return ss.str();
}

// --- Main ---

int main() {
    ConfigParser parser;
    std::string configPath = "config/config.txt";
    
    // Prosta detekcja ścieżki
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

    // 6. Konwersja Gridu na obiekty Car
    std::vector<Car> raceCars;
    for (const auto& pos : grid) {
        Car car(pos.driver, pos.team);
        raceCars.push_back(car);
    }

    // Pobranie liczby okrążeń z configu
    int laps = parser.getGlobalConfig().laps;
    if (laps <= 0) laps = 10;

    Race race(raceCars, selectedTrack, laps);
    race.run();

    return 0;
}
