#include <omp.h>
#include <stdio.h>
int main(void) {
 double start=omp_get_wtime();
 #pragma omp parallel
 {
 int th=omp_get_thread_num();
 printf("thread # %d\n",th);
 #pragma omp single
 { printf("Inside Single # %d\n", th); printf("Exiting Single\n"); }
 printf("Hi again from thread # %d\n",th);
 }
 printf("Elapsed: %.6f ms\n",1000*(omp_get_wtime()-start));
 return 0;
}
