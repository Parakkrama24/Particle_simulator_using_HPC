#include "Simulation_pthreads.h"

int main() {

    int numParticles = 100;
    double bounds = 100.0;
    double dt = 0.1;
    int numThreads = 4;
    int steps = 1000;

    SimulationPthreads sim(numParticles, bounds, dt);

    sim.initialize();
    sim.runParallel(numThreads, steps);

    return 0;
}