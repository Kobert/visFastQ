#include <math.h>

#include "referenceAssembly.h" 
#include "ref_math.h" 

//Binomial coefficient of n over k
long unsigned binom(unsigned int n,unsigned int k)
{
    long unsigned result=1;
    unsigned int bottom=k>n-k?n-k:k;
    int j, top = n;
    for(j=1; j<=bottom; j++,top--)
    {
        if(top%j==0)
        {
           result*=top/j;
        }else
        if(result%j==0)
        {
            result=result/j*top;
        }else
        {
            result=(result*top)/j;
        }
    }
    return result;
}


double Q2P(unsigned int q)
{
  
 return pow(10.0, (double)q/(-10));
  
}

unsigned int P2Q(double p)
{
  
 return -10.0*log((double)p)/log(10);
  
}