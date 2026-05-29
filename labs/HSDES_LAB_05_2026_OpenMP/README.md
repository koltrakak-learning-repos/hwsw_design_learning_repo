# Lab 05 — OpenMP on PULP

## Setup

Use the Docker setup in the same way as you did in the previous labs.

## Structure

Each subdirectory is a self-contained exercise taken from a past exam. For each exam, we work on Exercise 2. You can find all past exams of interest on Virtuale. The directory name encodes the exam date.

| Directory | Exam date | Exercise |
|-----------|-----------|---------|
| `01_17_07_2025/` | 17 Jul 2025 | OpenMP reduction (`analyze_data`) |
| `02_20_06_2025/` | 20 Jun 2025 | Bare metal parallelization of FIR filter |
| `03_10_07_2024/` | 10 Jul 2024 | Bare metal replacement of `#pragma omp for` |
| `04_12_06_2024/` | 12 Jun 2024 | OpenMP parallelization of FIR filter |
| `05_12_06_2023/` | 12 Jun 2023 | OpenMP parallelization with loop-carried dependency |

## Build and run

From inside an exercise directory:

```bash
make clean all run CORES=8
```

`CORES` can be any value from 1 upwards. Start with `CORES=1` for the sequential baseline, then increase to measure parallel speedup.

Performance counters are enabled by default (`-DPERF`). Cycle and instruction counts are printed per core after execution.
