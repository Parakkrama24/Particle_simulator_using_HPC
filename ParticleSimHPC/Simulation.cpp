#include "Simulation.h"
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <omp.h>

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
// Update – parallel
// ─────────────────────────────────────────────
void Simulation::updateParallel()
{
#pragma omp parallel for schedule(static)
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

    std::cout << "Frames completed: " << frames << std::endl;
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
        << "  Frame: " << std::setw(8) << step
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
void Simulation::runInfinite(bool parallelMode, const std::string& csvPath)
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

    // ── Static banner (printed once, stays above the live block) ─────────
    std::cout << "==============================\n"
        << " Infinite mode  |  CSV: " << csvPath << "\n"
        << " Press Ctrl+C to stop\n"
        << "==============================\n";

    const int N = (int)particles.size();

    // ── Main loop ─────────────────────────────────────────────────────────
    while (true)
    {
        // 1. Advance physics
        if (parallelMode) updateParallel();
        else              updateSerial();

        // 2. Overwrite CSV in-place
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

        // 3. Live console display for particles 1–10
        printConsoleSnapshot();

        step++;
    }

    csvFile.close();
}