# Lab 05 — measured results

These measurements were made in the assistant-hosted Ubuntu Linux environment with GCC 13.3.0, using `-O2 -std=c11 -fopenmp`. They are not user EC2 or university cluster results. Each timing below is the median of three runs; shared host load affects performance. See results/environment.txt and raw benchmarks.txt for details.

## Correctness

- Exercises 1–4: directive output properties and unique iteration pairs checked.
- Fibonacci: serial/task results match; base and cutoff cases pass.
- Arrays: all 1,000,000 parallel results match the serial results.
- Supplied Mandelbrot: outside count **2,926,731**, inside count **1,073,269**, area **1.509284531250** for all tested mappings and thread counts.

## Mandelbrot performance

| Mode | Threads | Schedule | Median ms | Speedup vs serial |
|---|---:|---|---:|---:|
| serial | 1 | serial loop | 10425.945 | 1.00× |
| manual | 1 | cyclic rows | 9194.541 | 1.13× |
| manual | 2 | cyclic rows | 5138.766 | 2.03× |
| manual | 3 | cyclic rows | 3130.932 | 3.33× |
| manual | 4 | cyclic rows | 2687.581 | 3.88× |
| runtime | 4 | static | 4202.091 | 2.48× |
| runtime | 4 | static,1 | 2704.973 | 3.85× |
| runtime | 4 | static,8 | 2919.857 | 3.57× |
| runtime | 4 | dynamic,1 | 2566.312 | 4.06× |
| runtime | 4 | guided,1 | 2683.599 | 3.89× |

In this run, four-thread manual mapping achieved 3.88× speedup. The fastest tested runtime schedule was `dynamic,1` at 2566.312 ms. This is specific to this host and workload; remeasure on the submission machine.

## Fibonacci and array performance

| Program | Threads | Median serial ms | Median parallel ms | Ratio serial/parallel |
|---|---:|---:|---:|---:|
| Fibonacci F(35) | 1 | 17.374 | 19.403 | 0.90× |
| Arrays N=1,000,000 | 1 | 4.727 | 5.002 | 0.95× |
| Fibonacci F(35) | 2 | 19.367 | 10.807 | 1.79× |
| Arrays N=1,000,000 | 2 | 3.712 | 3.701 | 1.00× |
| Fibonacci F(35) | 3 | 18.733 | 14.469 | 1.29× |
| Arrays N=1,000,000 | 3 | 5.177 | 13.897 | 0.37× |
| Fibonacci F(35) | 4 | 19.469 | 9.148 | 2.13× |
| Arrays N=1,000,000 | 4 | 3.913 | 3.660 | 1.07× |

Short array timings include allocation first-touch effects and OpenMP startup; the serial version can also be automatically vectorized by GCC. These results do not isolate SIMD gains.

## Still outstanding

The final Molecular Dynamics exercise needs its original source. Exercises 1 and 4 were executed directly on Linux here; the required qsub cluster submission remains for your course environment. Screenshots are not explicitly required in the pasted instructions.
