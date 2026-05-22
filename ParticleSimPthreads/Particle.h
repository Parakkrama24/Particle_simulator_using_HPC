// he underlying data structure for each point is a simple struct defined in Particle.h

#pragma once
#include <iostream>

struct Particle {
    double x, y, z;        // Position
    double vx, vy, vz;     // Velocity

    Particle(double x, double y, double z,
        double vx, double vy, double vz)
        : x(x), y(y), z(z), vx(vx), vy(vy), vz(vz) {
    }
};