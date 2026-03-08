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

    Simulation(int numParticles, double bounds, double dt);

    void initialize();

    void updateSerial();
    void updateParallel();

    void runBenchmark(int seconds, bool parallelMode);

private:
    std::ofstream csvFile;
};