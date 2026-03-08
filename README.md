# 🧪 3D Particle Simulation

A high-performance 3D particle simulation written in C++ with **OpenMP** parallelization, real-time console output, and live CSV position export.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Requirements](#requirements)
- [Building](#building)
- [Usage](#usage)
- [Modes](#modes)
- [CSV Output Format](#csv-output-format)
- [Console Preview](#console-preview)
- [Configuration](#configuration)

---

## Overview

This project simulates N particles moving in a bounded 3D box. Particles bounce off walls with velocity reflection. It supports two execution modes:

- **Benchmark mode** — runs for a fixed number of seconds and reports FPS (serial vs parallel comparison)
- **Infinite mode** — runs forever, continuously overwriting a CSV snapshot of all particle positions and displaying a live console preview

---

## Features

- ⚡ **OpenMP parallelization** — multi-threaded particle updates via `#pragma omp parallel for`
- 📊 **Serial vs parallel benchmark** — side-by-side FPS comparison
- ♾️ **Infinite mode** — continuous simulation with no end condition
- 📁 **Live CSV export** — fixed-size file overwritten in-place every frame (never grows)
- 🖥️ **Real-time console preview** — displays positions of particles 0–9 updating live using ANSI escape codes
- 🎯 **Reproducible initialization** — fixed random seed (`srand(42)`) for consistent starting conditions

---

## Project Structure

```
.
├── Particle.h         # Particle struct (x, y, z, vx, vy, vz)
├── Simulation.h       # Simulation class declaration
├── Simulation.cpp     # Core simulation logic, benchmark & infinite modes
└── main.cpp           # Entry point and configuration
```

---

## Requirements

| Tool | Version |
|------|---------|
| C++ compiler | C++11 or later (GCC / Clang / MSVC) |
| OpenMP | 3.0+ |

**Linux / macOS (GCC):**
```bash
sudo apt install g++ libomp-dev   # Ubuntu/Debian
brew install libomp               # macOS
```

---

## Building

```bash
# GCC
g++ -O2 -fopenmp -o simulation main.cpp Simulation.cpp

# Clang (macOS)
clang++ -O2 -Xpreprocessor -fopenmp -lomp -o simulation main.cpp Simulation.cpp
```

---

## Usage

```bash
./simulation
```

Output goes to the terminal. In infinite mode, `particles.csv` is created in the working directory.

---

## Modes

Controlled by a single boolean in `main.cpp`:

```cpp
const bool INFINITE_MODE = false;   // false = benchmark, true = infinite
```

### Benchmark Mode (`false`)
Runs a timed serial pass then a timed parallel pass and prints FPS for each.

```
==============================
SERIAL EXECUTION
==============================
Frames completed: 1423
Average FPS: 142.3

==============================
PARALLEL EXECUTION
==============================
Frames completed: 4981
Average FPS: 498.1
```

### Infinite Mode (`true`)
Runs indefinitely. Every frame:
1. Updates all particle positions
2. Overwrites `particles.csv` with the current snapshot
3. Redraws the console preview

Stop with **Ctrl+C**.

---

## CSV Output Format

The CSV file has exactly **N + 1 lines** and never changes size — positions are overwritten in-place each frame using fixed-width formatting and `seekp`.

```
particle_id,           x,           y,           z
      0,   +12.345678,   -98.765432,    +0.000001
      1,   -45.123456,   +67.891234,   -12.345678
    ...
    499,   +99.000000,    -1.234567,   +55.678901
```

This makes the file safe to poll from external tools (Python, Excel, etc.) without worrying about partial reads during a growing append.

---

## Console Preview

In infinite mode the terminal displays a live-updating table of the first 10 particles using ANSI escape codes to overwrite lines in-place (no scrolling):

```
==============================================================
  Infinite mode  |  CSV: particles.csv
  Total particles: 500  |  Preview: 0-9  |  Ctrl+C to stop
==============================================================
ID              X                  Y                  Z
--------------------------------------------------------------
0       +12.3451          -98.7654           +0.0001
1        -3.2100          +45.6789          -12.3456
2       +67.8901          +23.4567          +89.0123
...
9       -11.1234          +55.6789          -33.4567
--------------------------------------------------------------
Frame: 4821
```

---

## Configuration

All settings are at the top of `main.cpp`:

```cpp
const int    NUM_PARTICLES = 500;       // number of particles
const int    DURATION      = 10;        // benchmark duration in seconds
const double BOUNDS        = 100.0;     // box half-extent on each axis
const double DT            = 0.1;       // timestep per frame
const bool   INFINITE_MODE = false;     // false = benchmark, true = infinite
const bool   PARALLEL      = true;      // use OpenMP parallel update
const int    NUM_THREADS   = 4;         // OpenMP thread count
```

To change the CSV output path in infinite mode:
```cpp
sim.runInfinite(PARALLEL, "/path/to/output.csv");
```
