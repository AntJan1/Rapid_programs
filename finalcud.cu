#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <cuda.h>
#include <string.h>
#include <cuda_runtime.h>
#include "device_launch_parameters.h"

__global__ void vfun(short *vectrzy, long long n, long long *gpup, long long *xgpu,long long it,long long g)
{
    long long res; long long resx;long long rowx;long long col;long long row; long long size=(long long)pow(2,(n-1));
    long long i=blockIdx.x * blockDim.x + threadIdx.x+size/pow(2,it)*g;
    long long ig=blockIdx.x * blockDim.x + threadIdx.x;
    *(gpup+ig)=0;
    for(long long j=0;j<size;j++)
    {
        res=1;
        resx=0;
        col=(i*size+j)/size;
        row=(i*size+j)-size*col;
        rowx=j;
        for(long long k=0;k<(n-1)/2;k++)
        {
            res*=*(vectrzy+row%4+(col%4)*4);
            row/=4;
            col/=4;
        }
        for(long long k=0;k<(n-1)/2;k++)
        {
//             printf("%d",resx);
            resx+=*(xgpu+rowx%4);
//              printf("%d\n",resx);
            rowx/=4;
        }
        resx++;
        if(resx<0)
            resx*=-1;
     *(gpup+ig)+=res*resx;
    }
//     printf("%lld\n",*(gpup+i));
}

int main(int argv,char *argc[])
{
    FILE *wynik;
    wynik=fopen("eigenvalues.txt","w+");
    clock_t start = clock();
    long long n;
    long long *gpux;long long *gpup; short *vec3gpu;
    n=atoi(argc[1]);

    unsigned long long sn=pow(2,n-1);
    long long it=0;
    for(;sn>pow(2,28);)
    {
     sn/=2;
     it++;
    }
     unsigned long long size=sn*sizeof(long long);

         long long threadsperblock = (long long)pow(2,(n-1)/2);
    if(threadsperblock>1024)
        threadsperblock=1024;
    long long blockspergrid=(sn + threadsperblock -1) / threadsperblock;

    long long *p=(long long *)malloc(size);
    short *vec=(short *)malloc(16*sizeof(short));
    cudaMalloc((void **)&gpux, sizeof(long long)*4);
    cudaMalloc((void **)&gpup,size);
    cudaMalloc((void **)&vec3gpu,16*sizeof(short));

// Calculating eigenvalues
        *vec=1;*(vec+1)=1;*(vec+2)=1;*(vec+3)=1;*(vec+4)=1;*(vec+5)=1;*(vec+6)=-1;*(vec+7)=-1;*(vec+8)=1;*(vec+9)=-1;*(vec+10)=-1;*(vec+11)=1;*(vec+12)=1;*(vec+13)=-1;*(vec+14)=1;*(vec+15)=-1;
        *p=2;*(p+1)=0;*(p+2)=-2;*(p+3)=0;
        cudaMemcpy(gpux, p, sizeof(long long)*4, cudaMemcpyHostToDevice);
        cudaMemcpy(vec3gpu,vec,16*sizeof(short),cudaMemcpyHostToDevice);

        for(long long g=0;g<pow(2,it);g++)
        {
            vfun<<<blockspergrid,threadsperblock>>>(vec3gpu, n, gpup, gpux,it,g);
            cudaDeviceSynchronize();
            cudaMemcpy(p,gpup,size,cudaMemcpyDeviceToHost);

            for(long long i=0;i<sn;i++)
            {
                fprintf(wynik,"%lld\n",*(p+i));
                if(*(p+i)==0){
                printf("FATAL ERROR ");break;}
            }
        }
// clock
      clock_t end = clock();
      float t = (float)(end-start)/CLOCKS_PER_SEC;
      printf("%f\n",t);

//free memory
     cudaFree(gpux);
     cudaFree(gpup);
     cudaFree(vec3gpu);
     free(vec);
     free(p);
     fclose(wynik);
     cudaDeviceReset();
    return 0;
}
