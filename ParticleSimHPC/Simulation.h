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
    void update();
    void writeCSV();
    void runContinuous();

private:
    std::ofstream csvFile;
};