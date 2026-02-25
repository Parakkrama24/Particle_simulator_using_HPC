#include "Simulation.h"
#include <cstdlib>
#include <iostream>
#include <chrono>

Simulation::Simulation(int numParticles, double bounds, double dt)
    : bounds(bounds), dt(dt), step(0) {
    particles.resize(numParticles, Particle(0, 0, 0, 0, 0, 0));

    // Open CSV file once, write header
    csvFile.open("particles.csv");
    csvFile << "frame,particle_id,x,y,z,vx,vy,vz\n";
}

void Simulation::initialize() {
    srand(42);
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

void Simulation::writeCSV() {
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
    csvFile.flush();  // Ensure data is written immediately each frame
}

void Simulation::runContinuous() {
    std::cout << "Simulation running... Writing to particles.csv\n";
    std::cout << "Press Ctrl+C to stop.\n\n";

    while (true) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        update();
        writeCSV();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> frameTime = frameEnd - frameStart;

        std::cout << "Frame " << step << " written | Time: "
            << frameTime.count() << " ms\n";
    }
}

//Simulation::~Simulation() {
//    if (csvFile.is_open()) {
//        csvFile.close();
//        std::cout << "CSV file closed.\n";
//    }
//}