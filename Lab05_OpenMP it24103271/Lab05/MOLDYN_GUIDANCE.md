# Molecular dynamics — awaiting original source

This is implementation guidance, not a tested replacement for the missing `MolDyn/forces.c` and associated simulation. Upload the lecturer's ZIP to complete this exercise. Preserve the serial version before editing.

In the usual pair-force routine, the two sum-reduction variables are potential energy and virial (often called `epot` and `vir`). Confirm the actual names and scaling conventions in the source. Pair displacement, squared distance, inverse-distance powers, pair-force components, temporary accumulators and inner-loop indices must be private. Position arrays and constants are shared read-only; the force array is shared and written.

The essential pattern is:

```c
/* Illustrative structure only; not standalone compilable code. */
#pragma omp parallel for reduction(+:epot,vir) schedule(static,chunk)
for (int i = 0; i < npart; ++i) {
    for (int j = i + 1; j < npart; ++j) {
        /* Private temporaries: minimum-image displacement, distance,
           original cutoff test, original LJ energy and force calculation. */
        epot += pair_energy;
        vir  += pair_virial;
        #pragma omp critical(force_update)
        {
            f[3*i]   += fx; f[3*i+1] += fy; f[3*i+2] += fz;
            f[3*j]   -= fx; f[3*j+1] -= fy; f[3*j+2] -= fz;
        }
    }
}
```

Every concurrent update to the same force element must use the SAME named critical region, including any end-of-outer-loop additions of locally accumulated forces. Protecting only updates to particle j but leaving updates to particle i unprotected is still a race: another iteration can update that particle as its j. Initialize/zero shared forces before the parallel calculation. Retain the end-of-loop barrier before forces are consumed.

A critical region permits only one thread at a time to execute its block. It makes the grouped updates mutually exclusive; this exercise requests critical rather than replacing it with the atomic directive. A critical region around the entire outer loop would be correct for updates but would largely serialize the computation, so protect only the shared update operations.

Do not guess the lecturer's Lennard-Jones normalization, force sign, cutoff convention or virial definition. Those must stay exactly as in its serial routine. Some source layouts accumulate i forces locally to reduce locking; adapt protection to all shared writes after inspection.

Validation after editing:

1. Compile the untouched serial version and save its complete output.
2. Compile the parallel version with the same optimization flags plus `-fopenmp`.
3. Run identical particle count, timestep, initial conditions and step count using 2, 3 and 4 threads.
4. Compare every printed step's potential/kinetic/total energy, virial and temperature to the serial output. Use absolute-plus-relative floating-point tolerances justified by output precision, e.g. `abs(a-b) <= atol + rtol*max(abs(a),abs(b))`. Do not automatically demand bit-for-bit equality: the sum/update order changes. Long chaotic trajectories may magnify small rounding differences, requiring short-run/force-level validation as well.
5. Repeat using `schedule(static,1)`, `static,4`, `static,8`, `static,16` and the same thread counts. Record median calculation times from at least three runs.

For pair loops with j>i, early i iterations have more work than later ones. Small cyclic static chunks may improve load balance compared with large contiguous blocks. Very small chunks and lock contention can increase overhead; the actual best chunk size must be measured. No MD timings or correctness claims can be supplied until the original program is available and run.
