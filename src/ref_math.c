#include <math.h>

#include "referenceAssembly.h" 
#include "ref_math.h" 
#include "ref_slidingWindow.h"

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

// Returns the probability of randomly observing k matching windows in a sequence of length length, given a reference sequence of length refLength
double probRandom(setting s, unsigned int k, unsigned int refLength, unsigned int length)
{
  
  if(k == 0)
    return 1.0;
    
  unsigned int i;
  unsigned int fs = basesPerWindow();
  
  if(length < fs)
    return 0.0;
  
  double result;
  
  //chance of a single window matching a certain reference position
  double fourthToEight = pow(0.25, fs);
  //chance of not completely matching
  double C = 1 - fourthToEight;
  double Sum = 0;

  //Number of fragments in the sequence
  int l = floor((double)length/fs);
  
  //Sum up the probability of observing k or more random hits at a single position in the reference sequence.
  for(i = k; i<=l; i++)
  {
  Sum +=  binom(l,i)* pow(fourthToEight,i) * pow(C,l-i);   
  }
  
  // Chance of all positions having LESS than k hits
  double f = pow((1-Sum),(refLength - length +1));
  
  // Chance of at least one position having at least k hits.
  result = 1 - f;

 return result; 
}