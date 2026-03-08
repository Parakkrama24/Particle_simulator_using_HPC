#include "Simulation.h"
#include <iostream>
#include <omp.h>

int main()
{
    int particles = 100000;
    int duration = 10;

    double bounds = 100.0;
    double dt = 0.1;

    std::cout << "==============================\n";
    std::cout << "SERIAL EXECUTION\n";
    std::cout << "==============================\n";

    omp_set_num_threads(1);

    Simulation serialSim(particles, bounds, dt);
    serialSim.initialize();
    serialSim.runBenchmark(duration, false);

    std::cout << "\n==============================\n";
    std::cout << "PARALLEL EXECUTION\n";
    std::cout << "==============================\n";

    omp_set_num_threads(4);

    Simulation parallelSim(particles, bounds, dt);
    parallelSim.initialize();
    parallelSim.runBenchmark(duration, true);

    return 0;
}