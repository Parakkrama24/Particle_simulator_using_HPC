#include "Simulation.h"
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

// ─────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────
Simulation::Simulation(int numParticles, double bounds, double dt,
    bool infiniteMode)
    : bounds(bounds), dt(dt), step(0), infiniteMode(infiniteMode), rowBytes(0)
{
    particles.resize(numParticles, Particle(0, 0, 0, 0, 0, 0));
}

// ─────────────────────────────────────────────
// Initialize
// ─────────────────────────────────────────────
void Simulation::initialize()
{
    srand(42);
    for (int i = 0; i < (int)particles.size(); i++)
    {
        double vx = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        double vy = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        double vz = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        particles[i] = Particle(0, 0, 0, vx, vy, vz);
    }
}

// ─────────────────────────────────────────────
// Update – serial
// ─────────────────────────────────────────────
void Simulation::updateSerial()
{
    for (int i = 0; i < (int)particles.size(); i++)
    {
        Particle& p = particles[i];
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;

        if (p.x > bounds || p.x < -bounds) p.vx = -p.vx;
        if (p.y > bounds || p.y < -bounds) p.vy = -p.vy;
        if (p.z > bounds || p.z < -bounds) p.vz = -p.vz;
    }
}

// ─────────────────────────────────────────────
// Update Worker for pthreads
// ─────────────────────────────────────────────
void* Simulation::updateWorker(void* arg)
{
    ThreadData* data = (ThreadData*)arg;
    Simulation* sim = data->sim;
    
    for (int i = data->startIdx; i < data->endIdx; i++)
    {
        Particle& p = sim->particles[i];
        p.x += p.vx * sim->dt;
        p.y += p.vy * sim->dt;
        p.z += p.vz * sim->dt;

        if (p.x > sim->bounds || p.x < -sim->bounds) p.vx = -p.vx;
        if (p.y > sim->bounds || p.y < -sim->bounds) p.vy = -p.vy;
        if (p.z > sim->bounds || p.z < -sim->bounds) p.vz = -p.vz;
    }
    return NULL;
}

// ─────────────────────────────────────────────
// Update – parallel (pthreads)
// ─────────────────────────────────────────────
void Simulation::updateParallel()
{
    pthread_t* threads = new pthread_t[numThreads];
    ThreadData* threadData = new ThreadData[numThreads];
    
    int numParticles = (int)particles.size();
    int particlesPerThread = numParticles / numThreads;
    
    for (int i = 0; i < numThreads; i++)
    {
        threadData[i].sim = this;
        threadData[i].startIdx = i * particlesPerThread;
        threadData[i].endIdx = (i == numThreads - 1) ? numParticles : (i + 1) * particlesPerThread;
        
        pthread_create(&threads[i], NULL, updateWorker, &threadData[i]);
    }
    
    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    delete[] threads;
    delete[] threadData;
}

// ─────────────────────────────────────────────
// Finite benchmark mode (unchanged)
// ─────────────────────────────────────────────
void Simulation::runBenchmark(int seconds, bool parallelMode)
{
    int frames = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (true)
    {
        if (parallelMode) updateParallel();
        else              updateSerial();

        frames++;
        step++;

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - start;
        if (elapsed.count() >= seconds) break;
    }

    std::cout << "Iterations completed: " << frames << std::endl;
    std::cout << "Average FPS: " << frames / (double)seconds << std::endl;
}

// ─────────────────────────────────────────────
// printConsoleSnapshot
//
// Shows particles 1–10 (index 0–9) in the console.
// After the first frame, the cursor moves back up and rewrites
// the same lines — so the block updates in-place, no scrolling.
//
// Output looks like:
//
//  Frame:       42   (showing particles 1 – 10)
//    ID              X              Y              Z
//     1      +12.3456      -98.7654       +0.0001
//     2       -4.5123      +67.8912      -12.3456
//   ...
// ─────────────────────────────────────────────
void Simulation::printConsoleSnapshot()
{
    const int CONSOLE_ROWS = 10;
    const int show = std::min(CONSOLE_ROWS, (int)particles.size());

    // Total lines printed each call:  1 frame line + 1 column header + show data lines
    const int TOTAL_LINES = 2 + show;

    // Move cursor up to overwrite previous block (skip on first frame)
    if (step > 0)
        std::cout << "\033[" << TOTAL_LINES << "A";

    // ── Frame counter line ────────────────────────────────────────────────
    std::cout << "\033[2K"   // erase line
        << "  Itaretions: " << std::setw(8) << step
        << "   (showing particles 1 – " << show << ")\n";

    // ── Column header ─────────────────────────────────────────────────────
    std::cout << "\033[2K"
        << std::left << std::setw(6) << "   ID"
        << std::right << std::setw(14) << "X"
        << std::setw(14) << "Y"
        << std::setw(14) << "Z"
        << "\n";

    // ── Data rows (particles 1–10, i.e. index 0–9) ───────────────────────
    for (int i = 0; i < show; i++)
    {
        const Particle& p = particles[i];
        std::cout << "\033[2K"
            << std::right
            << std::setw(5) << (i + 1) << "   "
            << std::setw(12) << std::fixed << std::setprecision(4)
            << std::showpos << p.x
            << std::setw(14) << p.y
            << std::setw(14) << p.z
            << std::noshowpos << "\n";
    }

    std::cout.flush();
}

// ─────────────────────────────────────────────
// Infinite mode
// ─────────────────────────────────────────────
void Simulation::runInfinite(bool parallelMode, int itaretionCount, const std::string& csvPath)
{
    const int ID_W = 7;
    const int VAL_W = 13;
    const int PREC = 6;

    rowBytes = ID_W + 1 + VAL_W + 1 + VAL_W + 1 + VAL_W + 1;

    std::string header = "particle_id,x,y,z";
    while ((int)header.size() < rowBytes - 1) header += ' ';
    header += '\n';
    if ((int)header.size() != rowBytes)
    {
        std::cerr << "Header size mismatch!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    OpenCsv(csvPath, header, ID_W, VAL_W, PREC);

    // ── Static banner ─────────────────────────────────────────────────────
    std::cout << "==============================\n"
        << " Infinite mode  |  CSV: " << csvPath << "\n"
        << " Press Ctrl+C to stop\n"
        << "==============================\n";

    // ── Benchmark comparison block (runs once if itaretionCount != 0) ─────
    if (itaretionCount != 0)
    {
        LimitIterationMethod(itaretionCount);
    }

    const int N = (int)particles.size();

    // ── Main infinite loop ────────────────────────────────────────────────
    while (true)
    {
        if (parallelMode) updateParallel();
        else              updateSerial();

        // Overwrite CSV in-place
        csvFile.seekp(rowBytes, std::ios::beg);
        for (int i = 0; i < N; i++)
        {
            const Particle& p = particles[i];
            csvFile << std::setw(ID_W) << i << ','
                << std::setw(VAL_W) << std::fixed << std::setprecision(PREC)
                << std::showpos << p.x << ','
                << std::setw(VAL_W) << p.y << ','
                << std::setw(VAL_W) << p.z
                << std::noshowpos << '\n';
        }
        csvFile.flush();

        printConsoleSnapshot();
        step++;
    }

    csvFile.close();
}
void Simulation::LimitIterationMethod(int itaretionCount)
{
    std::cout << "\n--- Benchmarking " << itaretionCount
        << " iterations (Serial vs Parallel) ---\n";

    // Save current particle state so we run both methods on identical data
    std::vector<Particle> savedState = particles;

    // ── Serial run ────────────────────────────────────────────────────
    particles = savedState;
    auto serialStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < itaretionCount; i++)
        updateSerial();
    auto serialEnd = std::chrono::high_resolution_clock::now();
    double serialSec = std::chrono::duration<double>(serialEnd - serialStart).count();

    // ── Parallel run ──────────────────────────────────────────────────
    particles = savedState;
    auto parallelStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < itaretionCount; i++)
        updateParallel();
    auto parallelEnd = std::chrono::high_resolution_clock::now();
    double parallelSec = std::chrono::duration<double>(parallelEnd - parallelStart).count();

    // Restore state to what parallel left off on (fair starting point)
    // (particles already hold parallel's final state — no extra copy needed)

    double speedup = serialSec / parallelSec;

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Serial   time : " << std::setw(10) << serialSec << " s"
        << "  (" << std::setprecision(1) << (itaretionCount / serialSec)
        << " iter/s)\n";
    std::cout << std::setprecision(4);
    std::cout << "  Parallel time : " << std::setw(10) << parallelSec << " s"
        << "  (" << std::setprecision(1) << (itaretionCount / parallelSec)
        << " iter/s)\n";
    std::cout << std::setprecision(4);
    std::cout << "  Speedup       : " << std::setw(10) << speedup << "x  ";

    if (speedup >= 1.0)
        std::cout << "(Parallel is faster)\n";
    else
        std::cout << "(Serial is faster — consider thread overhead)\n";

    std::cout << "  Pthreads      : " << numThreads << "\n";
    std::cout << "----------------------------------------------\n\n";
}
//Open Csv
void Simulation::OpenCsv(const std::string& csvPath, std::string& header, const int ID_W, const int VAL_W, const int PREC)
{
    // ── Create & pre-fill CSV ─────────────────────────────────────────────
    {
        std::ofstream init(csvPath, std::ios::trunc);
        if (!init.is_open())
        {
            std::cerr << "ERROR: cannot open " << csvPath << std::endl;
            std::exit(EXIT_FAILURE);
        }
        init << header;
        const int N = (int)particles.size();
        for (int i = 0; i < N; i++)
        {
            init << std::setw(ID_W) << i << ','
                << std::setw(VAL_W) << std::fixed << std::setprecision(PREC)
                << std::showpos << 0.0 << ','
                << std::setw(VAL_W) << 0.0 << ','
                << std::setw(VAL_W) << 0.0
                << std::noshowpos << '\n';
        }
    }

    csvFile.open(csvPath, std::ios::in | std::ios::out | std::ios::binary);
    if (!csvFile.is_open())
    {
        std::cerr << "ERROR: cannot reopen " << csvPath << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
