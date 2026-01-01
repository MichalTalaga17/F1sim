#include "../include/Race.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>

// Powrót do system("clear") dla pełnego czyszczenia konsoli na żądanie użytkownika
#ifdef _WIN32
    #define CLEAR_CMD "cls"
#else
    #define CLEAR_CMD "clear"
#endif

Race::Race(const std::vector<Car>& c, const Track& t, int laps) 
    : cars(c), track(t), totalLaps(laps) {
    std::random_device rd;
    rng.seed(rd());
}

void Race::run() {
    double dt = 0.5; 
    
    std::cout << "\033[2J\033[1;1H" << std::flush;

    while (!raceFinished) {
        bool allFinished = true;

        int stepsPerFrame = 100;

        for (int step = 0; step < stepsPerFrame; ++step) {
            for (size_t i = 0; i < cars.size(); ++i) {
                if (cars[i].getCurrentLap() <= totalLaps) {
                    allFinished = false;
                    
                    const Car* carAhead = nullptr;
                    if (i > 0) {
                        if (!cars[i-1].hasFinished(totalLaps)) {
                            carAhead = &cars[i-1];
                        }
                    }

                    cars[i].update(dt, track, totalLaps, WeatherType::Sunny, rng, carAhead);
                } else if (!cars[i].hasFinished(totalLaps)) {
                    double expectedTotalDist = (double)totalLaps * track.totalLength;
                    double overshoot = cars[i].getTotalDistance() - expectedTotalDist;
                    
                    double speed = cars[i].getCurrentSpeed();
                    if (speed < 1.0) speed = 1.0;

                    double timeCorrection = overshoot / speed;
                    if (timeCorrection > dt) timeCorrection = dt;
                    if (timeCorrection < 0) timeCorrection = 0;

                    cars[i].completeRace(raceTime - timeCorrection);
                }
            }
            raceTime += dt;
            if (allFinished) break; 
        }
        
        allFinished = true; 
        for(const auto& car : cars) {
            if(!car.hasFinished(totalLaps)) {
                allFinished = false; 
                break;
            }
        }

        if (allFinished) {
            raceFinished = true;
        }

        updateLeaderboard();
        printStatus();
        
        if (!raceFinished) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    
    std::cout << "\n\n   >>> RACE FINISHED! <<<\n";
    updateLeaderboard();
    printStatus();

    std::cout << "\n=== STRATEGY SUMMARY ===\n";
    for (const auto& car : cars) {
         std::cout << std::left << std::setw(20) << car.getDriver().name 
                   << ": " << car.getStrategyString() << "\n";
    }
}

void Race::updateLeaderboard() {
    std::sort(cars.begin(), cars.end(), [this](const Car& a, const Car& b) {
        bool aFin = a.hasFinished(totalLaps);
        bool bFin = b.hasFinished(totalLaps);
        
        if (aFin && !bFin) return true;
        if (!aFin && bFin) return false;
        
        if (aFin && bFin) {
            if (a.getFinishTime() != b.getFinishTime()) {
                return a.getFinishTime() < b.getFinishTime();
            }
            return a.getTotalDistance() > b.getTotalDistance();
        }

        return a.getTotalDistance() > b.getTotalDistance();
    });
}

void Race::printStatus() {
    std::cout << "\033[2J\033[1;1H" << std::flush;

    std::cout << "RACE TIME: " << std::fixed << std::setprecision(1) << raceTime << "s | LAPS: " << totalLaps << "\n";
    std::cout << "=======================================================================\n";
    std::cout << std::left << std::setw(4) << "POS" 
              << std::setw(18) << "DRIVER" 
              << std::setw(10) << "LAP"
              << std::setw(15) << "SECTOR" 
              << std::setw(10) << "GAP" 
              << "STATUS" << "\n";
    std::cout << "-----------------------------------------------------------------------\n";
    
    double leaderDist = cars[0].getTotalDistance();
    
    double avgLapTime = 0.0;
    if (cars[0].hasFinished(totalLaps)) {
        avgLapTime = cars[0].getFinishTime() / (double)totalLaps;
    }

    for (size_t i = 0; i < cars.size(); ++i) {
        std::string gapStr;

        if (cars[0].hasFinished(totalLaps) && cars[i].hasFinished(totalLaps)) {
            if (i == 0) {
                gapStr = "Leader";
            } else {
                double timeDiff = cars[i].getFinishTime() - cars[0].getFinishTime();
                
                if (avgLapTime > 0 && timeDiff > avgLapTime) {
                    int lapsDown = (int)(timeDiff / avgLapTime);
                    gapStr = "+" + std::to_string(lapsDown) + " LAP";
                    if (lapsDown > 1) gapStr += "S";
                } else {
                    std::stringstream ss;
                    ss << "+" << std::fixed << std::setprecision(3) << timeDiff << "s";
                    gapStr = ss.str();
                }
            }
        }
        else {
            double gapMeters = leaderDist - cars[i].getTotalDistance();
            
            if (i == 0) {
                gapStr = "Leader";
            } else {
                double gapSeconds = gapMeters / 60.0;
                
                if (gapMeters > track.totalLength) {
                    int laps = (int)(gapMeters / track.totalLength);
                    gapStr = "+" + std::to_string(laps) + " LAP";
                    if (laps > 1) gapStr += "S";
                } else {
                    std::stringstream ss;
                    ss << "+" << std::fixed << std::setprecision(3) << gapSeconds << "s";
                    gapStr = ss.str();
                }
            }
        }
        
        int progress = (cars[i].getLapDistance() / track.totalLength) * 15;
        std::string bar = "[";
        for(int k=0; k<15; ++k) {
             if (k < progress) bar += "=";
             else if (k == progress) bar += ">";
             else bar += " ";
        }
        bar += "]";

        std::cout << std::left << std::setw(4) << (i+1) 
                  << std::setw(18) << cars[i].getDriver().name 
                  << cars[i].getCurrentLap() << "/" << totalLaps << " " 
                  << std::setw(16) << bar 
                  << std::setw(10) << gapStr
                  << cars[i].getStatus() << "\n";
    }
}
