#pragma once
#include "Particle.h"
#include <vector>
#include <fstream>
#include <string>
#include <pthread.h>

struct ThreadData {
    class Simulation* sim;
    int startIdx;
    int endIdx;
};

class Simulation {
public:
    std::vector<Particle> particles;
    double bounds;
    double dt;
    int step;
    int numThreads = 4; // Default to 4 threads

    bool infiniteMode = false;

    Simulation(int numParticles, double bounds, double dt,
        bool infiniteMode = false);

    void initialize();
    void updateSerial();
    void updateParallel();
    void runBenchmark(int seconds, bool parallelMode);
	void runInfinite(bool parallelMode, int iterationCount,
        const std::string& csvPath = "particles.csv");

    void LimitIterationMethod(int itaretionCount);

    void OpenCsv(const std::string& csvPath, std::string& header, const int ID_W, const int VAL_W, const int PREC);

    // Static worker function for pthreads
    static void* updateWorker(void* arg);

private:
    std::fstream csvFile;
    int          rowBytes;

    // Prints particles 1-10 to the console, overwriting the same
    // lines every frame so the display updates in-place.
    void printConsoleSnapshot();
};