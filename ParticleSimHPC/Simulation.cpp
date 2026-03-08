#include "Simulation.h"
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <omp.h>

Simulation::Simulation(int numParticles, double bounds, double dt)
    : bounds(bounds), dt(dt), step(0)
{
    particles.resize(numParticles, Particle(0, 0, 0, 0, 0, 0));
}

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

void Simulation::runBenchmark(int seconds, bool parallelMode)
{
    int frames = 0;

    auto start = std::chrono::high_resolution_clock::now();

    while (true)
    {
        if (parallelMode)
            updateParallel();
        else
            updateSerial();

        frames++;

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - start;

        if (elapsed.count() >= seconds)
            break;
    }

    std::cout << "Frames completed: " << frames << std::endl;
    std::cout << "Average FPS: " << frames / (double)seconds << std::endl;
}