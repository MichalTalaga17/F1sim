#include <iostream>
#include <string>
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

    Car car;
    Race race(track, car);
    race.run();

    return 0;
}
