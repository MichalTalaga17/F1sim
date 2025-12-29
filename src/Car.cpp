#include "../include/Car.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Car::Car(const Driver& d, const Team& t) : driver(d), team(t) {
    // Startujemy z lekkim losowym opóźnieniem reakcji (0-0.2s w dystansie)
    // symulując start z pól, ale tutaj upraszczamy - wszyscy ruszają z 0
}

void Car::update(double dt, const Track& track, WeatherType weather) {
    if (finished) return;

    // 1. Obsługa Pit Stopu
    if (pitStopTimer > 0) {
        pitStopTimer -= dt;
        if (pitStopTimer <= 0) {
            // Koniec pitstopu - nowe opony
            tireHealth = 1.0;
            pitStopTimer = 0;
        }
        return; // Bolid stoi
    }

    // 2. Ustalenie, gdzie jesteśmy na torze (który segment)
    double distanceSearch = 0.0;
    const TrackSegment* currentSeg = &track.segments[0];
    const TrackSegment* nextSeg = nullptr;

    for (size_t i = 0; i < track.segments.size(); ++i) {
        distanceSearch += track.segments[i].length;
        if (distanceSearch >= lapDistance) {
            currentSeg = &track.segments[i];
            // Pobieramy następny segment (dla planowania hamowania)
            if (i + 1 < track.segments.size()) nextSeg = &track.segments[i+1];
            else nextSeg = &track.segments[0]; // Pętla toru
            break;
        }
    }

    // 3. Warunki pogodowe
    double weatherGrip = 1.0;
    if (weather == WeatherType::Rainy) weatherGrip = 0.85;
    else if (weather == WeatherType::Storm) weatherGrip = 0.70;

    // 4. Obliczanie fizyki
    // Target Speed wynika z limitu segmentu ORAZ limitu następnego segmentu (musimy wyhamować!)
    double limitCurrent = calculateTargetSpeed(*currentSeg, weatherGrip);
    double limitNext = nextSeg ? calculateTargetSpeed(*nextSeg, weatherGrip) : limitCurrent;

    double targetSpeed = limitCurrent;

    // Logika hamowania: Jeśli zbliżamy się do końca segmentu i następny jest wolniejszy, 
    // musimy zacząć zwalniać.
    double distToEndOfSeg = distanceSearch - lapDistance;
    double requiredBrakingDist = (pow(currentSpeed, 2) - pow(limitNext, 2)) / (2 * getEffectiveBraking(weatherGrip));
    
    if (currentSpeed > limitNext && distToEndOfSeg <= requiredBrakingDist * 1.1) {
        // Musimy hamować
        targetSpeed = limitNext;
    }

    // Aplikowanie sił
    if (currentSpeed < targetSpeed) {
        currentSpeed += getEffectiveAcceleration(weatherGrip) * dt;
        if (currentSpeed > targetSpeed) currentSpeed = targetSpeed;
    } else if (currentSpeed > targetSpeed) {
        currentSpeed -= getEffectiveBraking(weatherGrip) * dt;
        if (currentSpeed < targetSpeed) currentSpeed = targetSpeed;
    }

    // 5. Ruch
    double moveDist = currentSpeed * dt;
    lapDistance += moveDist;
    totalDistance += moveDist;

    // 6. Zużycie opon (uproszczone)
    // Zakręty zużywają opony szybciej
    double wearFactor = (currentSeg->type == "CORNER") ? 2.0 : 0.5;
    // Agresywni kierowcy zużywają szybciej
    double driverFactor = 1.0 + ((100 - driver.awareness) / 200.0); 
    tireHealth -= (0.0003 * wearFactor * driverFactor * dt); 
    if (tireHealth < 0.2) tireHealth = 0.2; // Klif oponiarski

    // 7. Przekroczenie linii mety
    if (lapDistance >= track.totalLength) {
        lapDistance -= track.totalLength;
        currentLap++;
        
        // Pit Stop Logic (Prosta strategia: Zjedź, jeśli opony < 40%)
        if (tireHealth < 0.40 && !finished) {
            // Czas postoju: Baza (20s) * TeamMultiplier + Losowość
            double baseTime = 22.0;
            pitStopTimer = baseTime * team.pitStopMultiplier;
        }
    }
}

double Car::calculateTargetSpeed(const TrackSegment& segment, double gripModifier) {
    if (segment.type == "STRAIGHT") {
        // Top Speed zależy od silnika i zużycia opon (wyjście na prostą)
        // Konwertujemy TopSpeed (umowna statystyka 0-100) na m/s
        // 100 -> ~340 km/h (94 m/s)
        double maxCarSpeed = (team.topSpeed / 100.0) * 94.0;
        return maxCarSpeed;
    } else {
        // Fizyka zakrętu: v = sqrt(r * g * mu)
        double radius = std::abs(segment.radius);
        if (radius < 1.0) radius = 1.0;
        
        // Grip: Baza opony * Grip Toru * Stan Opon * Umiejętności Kierowcy (Racecraft)
        double driverSkill = 0.8 + (driver.racecraft / 500.0); // 1.0 for 100 RAC
        double tireState = 0.5 + (tireHealth * 0.5); // Jak opona zdarta, trzyma gorzej
        double effectiveGrip = team.baseTireGrip * gripModifier * driverSkill * tireState;

        double vMax = std::sqrt(radius * 9.81 * effectiveGrip);
        if (vMax > 85.0) vMax = 85.0; // Limit fizyczny
        return vMax;
    }
}

double Car::getEffectiveAcceleration(double gripModifier) {
    // Acc stat (0-15) -> m/s^2
    double baseAcc = team.acceleration; 
    // Na zużytych oponach gorzej się przyspiesza
    return baseAcc * tireHealth * gripModifier;
}

double Car::getEffectiveBraking(double gripModifier) {
    // Braking stat (0-25) -> m/s^2
    double baseBrake = team.braking;
    return baseBrake * tireHealth * gripModifier;
}

std::string Car::getStatus() const {
    if (finished) return "FINISHED";
    if (pitStopTimer > 0) return "IN PIT";
    return "Lap " + std::to_string(currentLap);
}
