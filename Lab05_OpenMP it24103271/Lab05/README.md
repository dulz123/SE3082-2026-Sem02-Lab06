# Lab 05 — OpenMP

Prepared from the instructions identified by the user as Lab 05. Lab 06 is separate.

Exercises 1–7 are implemented. The final Molecular Dynamics exercise remains blocked by missing original source. No qsub job was submitted; use the included PBS example on your course cluster. Tests here are assistant-hosted Linux evidence, not the user's EC2 runs.

## What to do next — beginner steps

1. Download and extract Lab05_OpenMP.zip.
2. Obtain the missing MolDyn source for the final exercise; the supplied Mandelbrot code has now been incorporated.
3. Open a terminal in your Linux instance. On Ubuntu, install tools if missing:

```bash
sudo apt update
sudo apt install build-essential python3 unzip
```

On Amazon Linux, use `sudo dnf install gcc make libgomp python3 unzip` instead. Package installation requires appropriate instance permissions. You do not need IntelliJ; a Linux terminal is enough. An editor terminal is suitable only when it is connected to Linux, not a normal Windows PowerShell session.

4. Transfer the ZIP to that instance using your existing SSH/SCP access, then run:

```bash
unzip Lab05_OpenMP.zip
cd Lab05
bash run_all.sh
```

A typical SCP command from your own computer is below. Replace every placeholder; use `ec2-user` instead of `ubuntu` when appropriate. Do not send anyone your private key.

```bash
scp -i "YOUR_KEY.pem" Lab05_OpenMP.zip ubuntu@YOUR_INSTANCE_ADDRESS:~/
```

5. The script compiles eight programs, runs demonstrations, tests 1/2/3/4 threads, tries schedules, and records three benchmark repetitions. It writes your results under `results/`, replacing the bundled reference results. `mandelbrot_summary.csv` contains median times and speedups.
6. For a university PBS/OpenPBS cluster, copy the extracted folder there, change into it, review `cluster.pbs` resource syntax, and run:

```bash
qsub cluster.pbs
qstat
```

When finished, view the `openmp_lab.oJOBID` output file (actual name can vary). Some Torque clusters require `#PBS -l nodes=1:ppn=4` instead of the supplied `select` line. SGE also has qsub but uses a different script format; request the department's template if necessary. Ordinary EC2 does not normally provide a configured batch scheduler. Installing a qsub client alone does not create a cluster. Ask the lecturer whether direct EC2 execution is accepted for Exercises 1 and 4 if no cluster account is provided.
7. Save the command output and timing tables described below. Screenshots are optional: the provided sheet does not explicitly request them. Read the answers and be prepared to explain the code. Add your name/registration number and your own measured results to your submission in the format your lecturer requests.

## Exercise 1 — barrier

Every thread prints its first `thread #` line before reaching the barrier. The barrier prevents any thread from continuing beyond that point until all threads in the team have arrived. Therefore, all initial lines appear before any `Outside Barrier` or `Hi again` lines. The order of threads within each stage is not guaranteed. The barrier does not make thread 0 execute first or serialize the remaining prints. Each thread still prints its own Outside line before its own Hi line.

Run: `OMP_NUM_THREADS=4 ./bin/ex1_barrier` (use qsub for the required cluster evidence).

## Exercise 2 — single

Exactly one team thread executes the single block; it is not necessarily thread 0. `Inside Single` and `Exiting Single` appear once. Other threads skip the block and wait at its implicit end barrier, so every `Hi again` line follows `Exiting Single`. There is no entry barrier: the chosen thread can enter single before another thread has printed its initial line. Adding `nowait` would remove the end barrier, but this exercise does not use it.

Run: `OMP_NUM_THREADS=4 ./bin/ex2_single`.

## Exercise 3 — master

Only the primary thread, whose ID is 0, executes master. The two master-block messages appear once. Master has no implicit entry or exit barrier, so other threads may print `Hi again` before thread 0 finishes the master block. There is still an implicit barrier at the end of the parallel region. The supplied code was missing the brace ending the parallel region; return must be outside that region.

Run: `OMP_NUM_THREADS=4 ./bin/ex3_master`.

## Exercise 4 — outer loop and collapse

`parallel for` creates a team and distributes the five outer r iterations across it. For each assigned r, the same thread executes all ten k iterations serially. There are 50 printed pairs in total. At most five threads can do useful loop work. The requested team size comes from OpenMP settings and runtime limits; the loop does not automatically generate five threads.

`parallel for collapse(2)` combines the two iteration spaces into 50 logical iterations and distributes those across one team. Different k values of a given r can now be assigned to different threads. It does not create a new team per row or automatically create 50 threads. Exact assignments and output order are not prescribed by this code; add an explicit schedule for a controlled comparison. Both versions must print each (r,k) pair exactly once.

Run `ex4_outer` and `ex4_collapse` with `OMP_NUM_THREADS=4`; submit the cluster job for required qsub evidence.

## Exercise 5 — Fibonacci tasks

`serial_fib` is the original recursive algorithm. The parallel implementation creates one parallel team; one thread starts the recursive computation using single. Each recursive call above the cutoff creates two child tasks. `firstprivate(n)` gives each task its own captured n. `shared(i)` and `shared(j)` let the tasks write results back to the parent call's variables. `taskwait` completes both children before the parent adds their results, also keeping those variables alive until the writes finish. Tasks use the existing team; a task is not a new operating-system thread.

The cutoff of 20 avoids excessive task overhead for small subproblems. n=35 should produce 9227465 in both versions. The program accepts n=0..45 to limit recursive runtime; it computes F(n), matching the function in the question, rather than printing an entire sequence. Parallel overhead means speedup is not guaranteed.

Run: `OMP_NUM_THREADS=4 ./bin/ex5_fibonacci 35`.

## Exercise 6 — array multiplication using strip mining

The arrays contain 1,000,000 double elements. The outer loop processes strips of 256 elements. OpenMP distributes strips with a static schedule; an inner simd directive allows vectorization of independent multiplications. A strip size of 256 is a multiple of common double SIMD widths: 2 doubles for 128-bit, 4 for 256-bit, and 8 for 512-bit vectors. It is a strip-length choice, not a claim that malloc guarantees 64-byte alignment. No unsupported aligned clause is used. The final strip is bounded by N because N is not divisible by 256.

Each output index is written once, so no critical section is needed. The program checks every parallel element against the serial result and prints a checksum. Initialization, comparison and output are excluded from computation timing. Thread creation and worksharing overhead are included. Memory bandwidth, first-touch page allocation, caching and thread startup can dominate this small benchmark. Repeat measurements and do not claim universal speedup.

## Exercise 7 — supplied Mandelbrot code

`src/ex7_mandelbrot.c` preserves the supplied 2000 × 2000 grid, 2000 iterations, z=c initialization, 1e-7 offsets, escape test after updating z, and real/imaginary ranges. The supplied paste ended at an unfinished comment. The ending, area output and timer were added; the unused error variable was removed because no error estimator was supplied. A finite grid and iteration limit do not provide a rigorous error bound.

The serial mode loops over every row without a parallel region. The manual parallel mode starts a team before the outer loop and uses `omp_get_thread_num()` and `omp_get_num_threads()` to assign cyclic rows. With 1, 2 and 4 threads, row counts are exactly equal. With 3 threads, 2000 is not divisible by 3, so row counts are 667, 667 and 666. This is the closest possible balance while preserving the original grid and assigning whole outer iterations. Equal numbers of points do not imply equal computation, because escape iterations differ.

Each thread has its own `numoutside` reduction counter. Row indices, column indices, iter, z, c and ztemp are local/private. The read-only constants are shared by definition. The runtime mode supports extra scheduling experiments through `OMP_SCHEDULE`. All modes use identical numerical operations for each point. The final area is 2 × 2.5 × 1.125 × fraction of points that did not escape. Integer reductions make the outside count and resulting area exactly reproducible across thread counts with this program.

```bash
./bin/ex7_mandelbrot serial
OMP_NUM_THREADS=1 ./bin/ex7_mandelbrot manual
OMP_NUM_THREADS=2 ./bin/ex7_mandelbrot manual
OMP_NUM_THREADS=3 ./bin/ex7_mandelbrot manual
OMP_NUM_THREADS=4 ./bin/ex7_mandelbrot manual
OMP_NUM_THREADS=4 OMP_SCHEDULE=static,8 ./bin/ex7_mandelbrot runtime
```

`benchmark.py` runs three repetitions per configuration, verifies identical results, and writes median times, speedups and efficiency. Dynamic or cyclic mappings may improve balance but add overhead; use the measured table to assess them.

## Timing and performance discussion

The sheet's snippet has missing punctuation and mismatched variable names. Use:

```c
double tstart, tstop, tcalc;
tstart = omp_get_wtime();
/* computation */
tstop = omp_get_wtime();
tcalc = 1000.0 * (tstop - tstart); /* milliseconds */
```

Read both timestamps from the same thread. Speedup = serial time / parallel time. Efficiency = speedup / number of threads. Report the input size, compiler flags, CPU/allocated cores, actual team size, repetitions and median. A one-thread OpenMP run includes OpenMP overhead and is not the same as the true serial baseline. Printing-demo times mainly measure I/O and are not useful evidence of computational scalability. Shared/cloud CPUs and limited CPU quotas make timings noisy; more threads can be slower. Avoid `-ffast-math` for these correctness comparisons.

## Output evidence and final submission checklist

- Environment: `gcc --version`, `lscpu`, Linux terminal/instance identification.
- Build: successful `make` output.
- Exercises 1/4: qsub command, job ID and completed job output where required.
- Exercises 2/3: full output with four threads.
- Exercise 5: matching Fibonacci result and serial/parallel times.
- Exercise 6: `mismatches=0`, PASS and timings.
- Mandelbrot: 1/2/3/4-thread identical results, serial baseline, timing table and schedule comparison.
- Molecular dynamics: still requires the supplied ZIP, modifications and real serial/2/3/4-thread comparisons.
- Include source files, written explanations and your measured results as required by the portal. Do not submit assistant-hosted results as your own EC2 run.

## References

OpenMP API specification: https://www.openmp.org/spec-html/5.2/openmp.html
Taskwait: https://www.openmp.org/spec-html/5.2/openmpse93.html
Synchronization constructs (including master in earlier spec): https://www.openmp.org/spec-html/5.0/openmpse25.html
