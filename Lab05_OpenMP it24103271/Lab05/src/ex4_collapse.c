#include <omp.h>
#include <stdio.h>
int main(void) {
 double start=omp_get_wtime();
 #pragma omp parallel for collapse(2)
 for(int r=1;r<=5;r++)
  for(int k=1;k<=10;k++)
   printf("r = %d, k = %d, thread # = %d\n",r,k,omp_get_thread_num());
 printf("Elapsed: %.6f ms\n",1000*(omp_get_wtime()-start));
 return 0;
}
