#include "Simulation.h"
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <omp.h>

Simulation::Simulation(int numParticles, double bounds, double dt)
    : bounds(bounds), dt(dt), step(0) {

    particles.resize(numParticles, Particle(0, 0, 0, 0, 0, 0));

    csvFile.open("particles.csv");
    csvFile << "frame,particle_id,x,y,z,vx,vy,vz\n";
}

void Simulation::initialize() {
    srand(42);

#pragma omp parallel for
    for (int i = 0; i < (int)particles.size(); i++) {
        double vx = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        double vy = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        double vz = ((double)rand() / RAND_MAX) * 2.0 - 1.0;

        particles[i] = Particle(0, 0, 0, vx, vy, vz);
    }
}

void Simulation::update()
{
#pragma omp parallel
    {
        int tid = omp_get_thread_num();

#pragma omp for schedule(static)
        for (int i = 0; i < (int)particles.size(); i++)
        {
            Particle& p = particles[i];

            p.x += p.vx * dt;
            p.y += p.vy * dt;
            p.z += p.vz * dt;

            if (p.x > bounds || p.x < -bounds) p.vx = -p.vx;
            if (p.y > bounds || p.y < -bounds) p.vy = -p.vy;
            if (p.z > bounds || p.z < -bounds) p.vz = -p.vz;

            if (i % 20000 == 0)
            {
#pragma omp critical
                std::cout << "Thread " << tid
                    << " processing particle " << i
                    << std::endl;
            }
        }
    }

    step++;
}

void Simulation::writeCSV() {

    // Writing to file must stay SERIAL
    for (int i = 0; i < (int)particles.size(); i++) {

        csvFile << step << ","
            << i << ","
            << particles[i].x << ","
            << particles[i].y << ","
            << particles[i].z << ","
            << particles[i].vx << ","
            << particles[i].vy << ","
            << particles[i].vz << "\n";
    }

    csvFile.flush();
}

void Simulation::runContinuous() {

    std::cout << "Simulation running (OpenMP)...\n";
    std::cout << "Threads: " << omp_get_max_threads() << "\n\n";
    std::cout << "Max Threads: " << omp_get_max_threads() << std::endl;

    while (true) {

        auto frameStart = std::chrono::high_resolution_clock::now();

        update();
        writeCSV();

        auto frameEnd = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> frameTime = frameEnd - frameStart;

        std::cout << "Frame " << step
            << " | Time: "
            << frameTime.count()
            << " ms\n";
    }
}