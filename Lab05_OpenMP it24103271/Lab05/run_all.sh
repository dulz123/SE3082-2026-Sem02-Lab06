#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
make
mkdir -p results
export OMP_DYNAMIC=FALSE
{ date -u; uname -a; gcc --version | head -n 1; lscpu; } > results/environment.txt
for e in ex1_barrier ex2_single ex3_master ex4_outer ex4_collapse; do
 OMP_NUM_THREADS=4 ./bin/"$e" | tee "results/$e.txt"
done
python3 benchmark.py
