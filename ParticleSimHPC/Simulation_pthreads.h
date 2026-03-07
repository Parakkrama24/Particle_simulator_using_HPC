#pragma once
#include "Simulation.h"
#include <pthread.h>

struct ThreadData {
    Simulation* sim;
    int start;
    int end;
};

void* threadUpdate(void* arg);

class SimulationPthreads : public Simulation {

public:

    SimulationPthreads(int numParticles, double bounds, double dt)
        : Simulation(numParticles, bounds, dt) {}

    void runParallel(int numThreads, int steps);
};