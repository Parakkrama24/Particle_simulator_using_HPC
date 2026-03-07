#include "Simulation_pthreads.h"
#include <iostream>
#include <chrono>

void* threadUpdate(void* arg) {

    ThreadData* data = (ThreadData*)arg;
    Simulation* sim = data->sim;

    for (int i = data->start; i < data->end; i++) {

        Particle& p = sim->particles[i];

        p.x += p.vx * sim->dt;
        p.y += p.vy * sim->dt;
        p.z += p.vz * sim->dt;

        if (p.x > sim->bounds || p.x < -sim->bounds) p.vx = -p.vx;
        if (p.y > sim->bounds || p.y < -sim->bounds) p.vy = -p.vy;
        if (p.z > sim->bounds || p.z < -sim->bounds) p.vz = -p.vz;
    }

    pthread_exit(NULL);
}

void SimulationPthreads::runParallel(int numThreads, int steps) {

    std::cout << "Running Parallel Simulation with "
        << numThreads << " threads\n";

    int numParticles = particles.size();

    pthread_t threads[16];
    ThreadData threadData[16];

    auto startTime = std::chrono::high_resolution_clock::now();

    for (int s = 0; s < steps; s++) {

        int chunk = numParticles / numThreads;

        for (int t = 0; t < numThreads; t++) {

            threadData[t].sim = this;
            threadData[t].start = t * chunk;

            if (t == numThreads - 1)
                threadData[t].end = numParticles;
            else
                threadData[t].end = (t + 1) * chunk;

            pthread_create(&threads[t], NULL, threadUpdate, &threadData[t]);
        }

        for (int t = 0; t < numThreads; t++)
            pthread_join(threads[t], NULL);

        step++;
    }

    auto endTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> duration = endTime - startTime;

    std::cout << "Parallel Time: " << duration.count() << " seconds\n";
}