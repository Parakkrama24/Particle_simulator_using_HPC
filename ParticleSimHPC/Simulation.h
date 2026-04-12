#pragma once
#include "Particle.h"
#include <vector>
#include <fstream>
#include <string>

class Simulation {
public:
    std::vector<Particle> particles;
    double bounds;
    double dt;
    int step;

    bool infiniteMode = false;

    Simulation(int numParticles, double bounds, double dt,
        bool infiniteMode = false);

    void initialize();
    void updateSerial();
    void updateParallel();
    void runBenchmark(int seconds, bool parallelMode);
	void runInfinite(bool parallelMode, int iterationCount,
        const std::string& csvPath = "particles.csv");

    void OpenCsv(const std::string& csvPath, std::string& header, const int ID_W, const int VAL_W, const int PREC);

private:
    std::fstream csvFile;
    int          rowBytes;

    // Prints particles 1-10 to the console, overwriting the same
    // lines every frame so the display updates in-place.
    void printConsoleSnapshot();
};