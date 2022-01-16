#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "omp.h"
void nest(int *p,int *x,int k)
{
    int i=0;
    int pow=2;
    for(int w=1;w<k-1;w++)
    {
        pow*=2;
    }
 #pragma omp paraller for private(i,j)
    for(int j=0;j<pow;j++)
    {
        *(x+i++)=*(p+j)+2;
    }
 #pragma omp paraller for private(i,j)
    for(int j=0;j<pow;j++)
    {
        *(x+i++)=*(p+j);
    }
 #pragma omp paraller for private(i,j)
    for(int j=0;j<pow;j++)
    {
        *(x+i++)=*(p+j)-2;
    }
 #pragma omp paraller for private(i,j)
    for(int j=0;j<pow;j++)
    {
        *(x+i++)=*(p+j);
    }
}

void nestv(FILE *v,int *vec,FILE *p,int k)
{
    int c=1;
    int pow=2;
    int pp;
    int z=0;
    int j;
    for(int w=1;w<k-3;w++)
    {
        pow*=2;
    }
    #pragma omp paraller for private (j,g,i)
for(int i=0;i<pow;i++)
{
for(int g=0;g<16;g++)
{
      z=0;
      fseek(p,0,0);
      while(z<i*pow)
      {
          fscanf(p,"%d",&j);
          z++;
      }
#pragma omp paraller for private (j,g,i)
    for(int j=0;j<pow;j++)
        {
            fscanf(p,"%d",&pp);
            fprintf(v,"%d\n",pp*(*(vec+g)));
        }
}
}
}

int main(int argv,char *argc[])
{
    FILE *wynik;
    FILE *ve;
    ve=fopen("vector.txt","r+");
    wynik=fopen("eigenvalues.txt","a");
    clock_t start = clock();
    int n;
     n=atoi(argc[1]);
    int *x=aligned_alloc(1024,pow(2,n-1)*sizeof(int));
    int *p=aligned_alloc(1024,pow(2,n-1)*sizeof(int));
    int v;
    int *vec=aligned_alloc(1024,16*sizeof(int));
    FILE *vep;
    vep=fopen("vep.txt","r+");
    fseek(vep,0,0);
    *p=3;*(p+1)=1;*(p+2)=-1;*(p+3)=1;
    *vec=1;*(vec+1)=1;*(vec+2)=1;*(vec+3)=1;*(vec+4)=1;*(vec+5)=1;*(vec+6)=-1;*(vec+7)=-1;*(vec+8)=1;*(vec+9)=-1;*(vec+10)=-1;*(vec+11)=1;*(vec+12)=1;*(vec+13)=-1;*(vec+14)=1;*(vec+15)=-1;
      int  i=0;
       fseek(ve,0,0);
       fscanf(ve,"%d",&v);
       fscanf(ve,"%d",&v);
       fseek(vep,0,0);
       if(v!=1)
       {
     for(i=0;i<16;i++)
     {
           fprintf(vep,"%d\n",*(vec+i));
      }
       fseek(vep,0,0);
       }
    int j=0;
    int s;
    #pragma omp paraller for
    for(i=3;i<n;)
    {
        nest(p,x,i);
        i+=2;
        #pragma omp paraller for
        for(j=0;j<pow(2,i-1);j++)
        {
            *(p+j)=*(x+j);
        }
    }
    #pragma omp paraller for
    for(i=0;i<pow(2,n-1);i++)
    {
    if(*(x+i)<0)
        {
            *(x+i)*=-1;
        }
    }
    i=0;
    int c;
//     #pragma omp paraller for
//     for(j=5;j<=n;j+=2)
//     {

         fseek(ve,0,0);
         fseek(vep,0,0);
         nestv(ve,vec,vep,n);
        fseek(ve,0,0);
        fseek(vep,0,0);
         while(i<pow(2,2*(n-1)))
         {
             fscanf(ve,"%d",&c);
             fprintf(vep,"%d\n",c);
             i++;
         }
                 i=0;
//      }
   s=0;
    c=0;
    fseek(ve,0,0);

for(i=0;i<pow(2,n-1);i++)
   {
       #pragma omp paraller for reduction(+s)
        for(j=0;j<pow(2,n-1);j++)
        {
            fscanf(ve,"%d",&v);
            s+=*(x+j)*v;
        }
      fprintf(wynik,"%d\n",s);
        s=0;
   }
   clock_t end = clock();
   float t = (float)(end-start)/CLOCKS_PER_SEC;
   printf("%f\n",t);
     free(x);
     free(vec);
     fclose(vep);
     free(p);
     fclose(ve);
      fclose(wynik);
    return 0;
}


