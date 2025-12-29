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
    : cars(c), track(t), totalLaps(laps) {}

void Race::run() {
    // Krok czasowy 0.5s (wystarczająco dokładny, a szybki)
    double dt = 0.5; 
    
    // Czyścimy ekran raz na początku
    std::cout << "\033[2J\033[1;1H" << std::flush;

    while (!raceFinished) {
        bool allFinished = true;

        // ACCELERATED SIMULATION (TURBO MODE):
        // 100 kroków * 0.5s = 50 sekund wyścigu na klatkę
        int stepsPerFrame = 100;

        for (int i = 0; i < stepsPerFrame; ++i) {
            for (auto& car : cars) {
                if (car.getCurrentLap() <= totalLaps) {
                    allFinished = false;
                    car.update(dt, track, WeatherType::Sunny);
                } else if (!car.hasFinished(totalLaps)) {
                    // PRECYZYJNE WYLICZENIE CZASU METY (Interpolacja)
                    // Ponieważ krok to 0.5s, musimy cofnąć czas o tyle, ile bolid przejechał "za linię"
                    double expectedTotalDist = (double)totalLaps * track.totalLength;
                    double overshoot = car.getTotalDistance() - expectedTotalDist;
                    
                    // Zabezpieczenie przed dziwnymi wartościami (np. prędkość 0 na mecie)
                    double speed = car.getCurrentSpeed();
                    if (speed < 1.0) speed = 1.0;

                    double timeCorrection = overshoot / speed;
                    // Jeśli korekta jest większa niż krok (niemożliwe w teorii), przytnijmy
                    if (timeCorrection > dt) timeCorrection = dt;
                    if (timeCorrection < 0) timeCorrection = 0;

                    car.completeRace(raceTime - timeCorrection);
                }
            }
            raceTime += dt;
            // Jeśli wszyscy skończyli w trakcie tych kroków, przerywamy wcześniej
            if (allFinished) break; 
        }
        
        // Sprawdzamy stan 'allFinished' ponownie po pętli fizycznej (bo mogła się zmienić wewnątrz)
        // Musimy sprawdzić, czy faktycznie wszyscy skończyli
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

        // Aktualizacja i wyświetlanie
        updateLeaderboard();
        printStatus();
        
        if (!raceFinished) {
            // Czekamy 50ms (bardzo płynne, szybkie odświeżanie)
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    
    std::cout << "\n\n   >>> RACE FINISHED! <<<\n";
    updateLeaderboard();
    printStatus();
}

void Race::updateLeaderboard() {
    std::sort(cars.begin(), cars.end(), [this](const Car& a, const Car& b) {
        bool aFin = a.hasFinished(totalLaps);
        bool bFin = b.hasFinished(totalLaps);
        
        // 1. Jeden skończył, drugi nie -> ten co skończył jest wyżej
        if (aFin && !bFin) return true;
        if (!aFin && bFin) return false;
        
        // 2. Obaj skończyli -> decyduje CZAS (kto szybciej)
        if (aFin && bFin) {
            if (a.getFinishTime() != b.getFinishTime()) {
                return a.getFinishTime() < b.getFinishTime();
            }
            // Remis w czasie (ten sam tick) -> kto dalej zajechał za linię
            return a.getTotalDistance() > b.getTotalDistance();
        }

        // 3. Obaj jadą -> decyduje DYSTANS
        return a.getTotalDistance() > b.getTotalDistance();
    });
}

void Race::printStatus() {
    // ANSI: \033[2J (Clear Screen) + \033[1;1H (Move Cursor to Top-Left)
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
    
    // Obliczamy średni czas okrążenia lidera (jeśli skończył), żeby wiedzieć ile to "1 LAP"
    double avgLapTime = 0.0;
    if (cars[0].hasFinished(totalLaps)) {
        avgLapTime = cars[0].getFinishTime() / (double)totalLaps;
    }

    for (size_t i = 0; i < cars.size(); ++i) {
        std::string gapStr;

        // 1. SCENARIUSZ: Lider i Kierowca są na mecie (Koniec wyścigu dla nich)
        if (cars[0].hasFinished(totalLaps) && cars[i].hasFinished(totalLaps)) {
            if (i == 0) {
                gapStr = "Leader";
            } else {
                double timeDiff = cars[i].getFinishTime() - cars[0].getFinishTime();
                
                // Jeśli strata jest większa niż jedno kółko -> dublowanie
                if (avgLapTime > 0 && timeDiff > avgLapTime) {
                    int lapsDown = (int)(timeDiff / avgLapTime);
                    gapStr = "+" + std::to_string(lapsDown) + " LAP";
                    if (lapsDown > 1) gapStr += "S";
                } else {
                    // Formatowanie czasu (np. +12.4s)
                    std::stringstream ss;
                    ss << "+" << std::fixed << std::setprecision(3) << timeDiff << "s";
                    gapStr = ss.str();
                }
            }
        }
        // 2. SCENARIUSZ: W trakcie wyścigu (bazujemy na dystansie)
        else {
            double gapMeters = leaderDist - cars[i].getTotalDistance();
            
            if (i == 0) {
                gapStr = "Leader";
            } else {
                // Szacunkowy czas straty (zakładając prędkość ~60m/s)
                double gapSeconds = gapMeters / 60.0;
                
                // Jeśli strata dystansu jest ogromna (> długość toru)
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
        
        // Pasek postępu na okrążeniu
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
