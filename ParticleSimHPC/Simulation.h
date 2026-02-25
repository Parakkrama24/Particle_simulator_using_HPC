#pragma once
#include "Particle.h"
#include <vector>

class Simulation {
public:
    std::vector<Particle> particles;
    double bounds;
    double dt;
    int step;

    Simulation(int numParticles, double bounds, double dt);
    void initialize();
    void update();
    void printPositions();
    void runContinuous();   // New: runs indefinitely
};