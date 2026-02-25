#include "Simulation.h"
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>

Simulation::Simulation(int numParticles, double bounds, double dt)
    : bounds(bounds), dt(dt), step(0) {
    particles.resize(numParticles, Particle(0, 0, 0, 0, 0, 0));
}

void Simulation::initialize() {
    srand(42);  // Fixed seed for reproducibility
    for (auto& p : particles) {
        double vx = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        double vy = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        double vz = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
        p = Particle(0, 0, 0, vx, vy, vz);
    }
}

void Simulation::update() {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;

        if (p.x > bounds || p.x < -bounds) p.vx = -p.vx;
        if (p.y > bounds || p.y < -bounds) p.vy = -p.vy;
        if (p.z > bounds || p.z < -bounds) p.vz = -p.vz;
    }
    step++;
}

void Simulation::printPositions() {
    std::cout << "=== Frame " << step << " ===\n";
    for (int i = 0; i < (int)particles.size(); i++) {
        std::cout << "P" << i << ": ("
            << particles[i].x << ", "
            << particles[i].y << ", "
            << particles[i].z << ")\n";
    }
}

void Simulation::runContinuous() {
    while (true) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        update();
        printPositions();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frameTime = frameEnd - frameStart;

        std::cout << "Frame time: " << frameTime.count() << " ms\n\n";

        // Optional: cap to ~60fps (16ms per frame)
        // std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}