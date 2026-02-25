#include "Simulation.h"

int main() {
    int numParticles = 100;    // Keep low first to read output clearly
    double bounds    = 100.0;
    double dt        = 0.1;

    Simulation sim(numParticles, bounds, dt);
    sim.initialize();
    sim.runContinuous();      // Runs until you press Ctrl+C

    return 0;
}
