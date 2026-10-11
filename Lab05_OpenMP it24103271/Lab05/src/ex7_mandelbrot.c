/* Based on the user's Lab 05 serial source; grid and iterations preserved.
   Added: completed main/output, timing, manual and runtime scheduling modes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define NPOINTS 2000
#define MAXITER 2000
struct complex { double real; double imag; };
/* A row uses only local temporary variables and returns an integer count. */
static int outside_row(int i) {
    int numoutside=0;
    for(int j=0;j<NPOINTS;j++) {
        struct complex c,z;
        c.real=-2.0+2.5*(double)i/(double)NPOINTS+1.0e-7;
        c.imag=1.125*(double)j/(double)NPOINTS+1.0e-7;
        z=c;
        for(int iter=0;iter<MAXITER;iter++) {
            double ztemp=z.real*z.real-z.imag*z.imag+c.real;
            z.imag=z.real*z.imag*2+c.imag;
            z.real=ztemp;
            if(z.real*z.real+z.imag*z.imag>4.0e0) {
                numoutside++; break;
            }
        }
    }
    return numoutside;
}
int main(int argc,char **argv) {
    const char *mode=argc>1?argv[1]:"manual";
    int numoutside=0,team=1;
    double tstart=omp_get_wtime();
    if(strcmp(mode,"serial")==0) {
        for(int i=0;i<NPOINTS;i++) numoutside+=outside_row(i);
    } else if(strcmp(mode,"manual")==0) {
        #pragma omp parallel default(none) shared(team) reduction(+:numoutside)
        {
            int tid=omp_get_thread_num(),nt=omp_get_num_threads();
            #pragma omp single
            team=nt;
            /* Cyclic outer-loop mapping: row counts differ by at most one. */
            for(int i=tid;i<NPOINTS;i+=nt) numoutside+=outside_row(i);
        }
    } else if(strcmp(mode,"runtime")==0) {
        #pragma omp parallel default(none) shared(team) reduction(+:numoutside)
        {
            #pragma omp single
            team=omp_get_num_threads();
            #pragma omp for schedule(runtime)
            for(int i=0;i<NPOINTS;i++) numoutside+=outside_row(i);
        }
    } else {fprintf(stderr,"Usage: %s [serial|manual|runtime]\n",argv[0]);return 1;}
    double tstop=omp_get_wtime();
    /* Rectangle area is 2.5*1.125; double to include its reflection. */
    double area=2.0*2.5*1.125*(NPOINTS*(double)NPOINTS-numoutside)
                /(NPOINTS*(double)NPOINTS);
    printf("mode=%s threads=%d grid=%d iterations=%d outside=%d area=%.12f time_ms=%.6f\n",
           mode,team,NPOINTS,MAXITER,numoutside,area,1000*(tstop-tstart));
    return 0;
}
