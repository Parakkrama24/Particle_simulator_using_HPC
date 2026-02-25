#include "Simulation.h"

int main() {
    int numParticles = 100;
    double bounds = 100.0;
    double dt = 0.1;

    Simulation sim(numParticles, bounds, dt);
    sim.initialize();
    sim.runContinuous();

    return 0;
}