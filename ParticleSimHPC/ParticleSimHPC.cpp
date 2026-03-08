#include "Simulation.h"
#include <omp.h>
#include <iostream>

int main()
{
    int numThreads = 4;           // change for experiments
    omp_set_num_threads(numThreads);

    std::cout << "Using threads: " << numThreads << std::endl;

    int numParticles = 100000;    // increase workload
    double bounds = 100.0;
    double dt = 0.1;

    Simulation sim(numParticles, bounds, dt);

    sim.initialize();
    sim.runContinuous();

    return 0;
}