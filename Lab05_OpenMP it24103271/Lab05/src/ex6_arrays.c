#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define N 1000000
#define STRIP 256
int main(void) {
    double *a=malloc(N*sizeof(*a)),*b=malloc(N*sizeof(*b));
    double *serial=malloc(N*sizeof(*serial)),*c=malloc(N*sizeof(*c));
    if(!a||!b||!serial||!c) {free(a);free(b);free(serial);free(c);return 1;}
    for(int i=0;i<N;i++){a[i]=(i%100)*0.25;b[i]=(i%50)*0.5;}
    double start=omp_get_wtime();
    for(int i=0;i<N;i++) serial[i]=a[i]*b[i];
    double serial_ms=1000*(omp_get_wtime()-start);
    int team=0;
    start=omp_get_wtime();
    #pragma omp parallel default(none) shared(a,b,c,team)
    {
        #pragma omp single
        team=omp_get_num_threads();
        #pragma omp for schedule(static)
        for(int base=0;base<N;base+=STRIP) {
            int end=base+STRIP<N?base+STRIP:N;
            #pragma omp simd
            for(int i=base;i<end;i++) c[i]=a[i]*b[i];
        }
    }
    double parallel_ms=1000*(omp_get_wtime()-start),checksum=0;
    int errors=0;
    for(int i=0;i<N;i++){if(c[i]!=serial[i])errors++;checksum+=c[i];}
    printf("N=%d strip=%d threads=%d serial_ms=%.6f parallel_ms=%.6f checksum=%.3f mismatches=%d %s\n",
           N,STRIP,team,serial_ms,parallel_ms,checksum,errors,errors?"FAIL":"PASS");
    free(a);free(b);free(serial);free(c);return errors!=0;
}
