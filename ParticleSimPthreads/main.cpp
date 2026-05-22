#include "Simulation.h"
#include <iostream>

int main()
{
    // ── Configuration ────────────────────────────────────────────────────────
    const int    NUM_PARTICLES = 100000;
    const int    DURATION = 10;       // seconds (finite/benchmark mode only)
    const double BOUNDS = 100.0;
    const double DT = 0.1;
	const int ITERATION_COUNT = 1000; // Number of iterations to run in infinite mode before writing to CSV

    // ── Toggle this bool ─────────────────────────────────────────────────────
    //   false → timed benchmark (original behaviour, no CSV)
    //   true  → infinite loop, CSV has exactly NUM_PARTICLES rows,
    //            each row's x/y/z is overwritten every frame
    const bool INFINITE_MODE = true;

    // ── Parallel flag & thread count (applies to both modes) ─────────────────
    const bool PARALLEL = true;
    const int  NUM_THREADS = 4;
    // ─────────────────────────────────────────────────────────────────────────

    if (INFINITE_MODE)
    {
        std::cout << "==============================\n";
        std::cout << "INFINITE MODE  ("
            << (PARALLEL ? "PARALLEL" : "SERIAL") << ")\n";
        std::cout << "Particles : " << NUM_PARTICLES << "\n";
        std::cout << "==============================\n";

        Simulation sim(NUM_PARTICLES, BOUNDS, DT, /*infiniteMode=*/true);
        sim.numThreads = NUM_THREADS;
        sim.initialize();
        sim.runInfinite(PARALLEL, ITERATION_COUNT, "particles.csv");
    }
    else
    {
        std::cout << "==============================\n";
        std::cout << "SERIAL EXECUTION\n";
        std::cout << "==============================\n";
        Simulation serialSim(NUM_PARTICLES, BOUNDS, DT);
        serialSim.initialize();
        serialSim.runBenchmark(DURATION, false);

        std::cout << "\n==============================\n";
        std::cout << "PARALLEL EXECUTION\n";
        std::cout << "==============================\n";
        Simulation parallelSim(NUM_PARTICLES, BOUNDS, DT);
        parallelSim.numThreads = NUM_THREADS;
        parallelSim.initialize();
        parallelSim.runBenchmark(DURATION, true);
    }

    return 0;
}