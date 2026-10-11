#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
static long long serial_fib(int n) {
    return n<2 ? n : serial_fib(n-1)+serial_fib(n-2);
}
static long long task_fib(int n) {
    /* Avoid creating tiny tasks: all smaller subtrees run serially. */
    if(n<=20) return serial_fib(n);
    long long i,j;
    #pragma omp task shared(i) firstprivate(n)
    i=task_fib(n-1);
    #pragma omp task shared(j) firstprivate(n)
    j=task_fib(n-2);
    #pragma omp taskwait
    return i+j;
}
int main(int argc,char **argv) {
    int n=argc>1?atoi(argv[1]):35;
    if(n<0 || n>45) {fprintf(stderr,"Use n=0..45 (recursive cost grows rapidly).\n");return 1;}
    double start=omp_get_wtime();
    long long expected=serial_fib(n);
    double serial_ms=1000*(omp_get_wtime()-start);
    long long actual=0; int team=0;
    start=omp_get_wtime();
    #pragma omp parallel default(none) shared(n,actual,team)
    {
        #pragma omp single
        { team=omp_get_num_threads(); actual=task_fib(n); }
    }
    double parallel_ms=1000*(omp_get_wtime()-start);
    printf("n=%d threads=%d serial=%lld parallel=%lld serial_ms=%.6f parallel_ms=%.6f %s\n",
           n,team,expected,actual,serial_ms,parallel_ms,expected==actual?"PASS":"FAIL");
    return expected!=actual;
}
