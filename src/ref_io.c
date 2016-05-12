#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <limits.h>

#include "referenceAssembly.h"
#include "ref_math.h"


void readToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length)
{
  
  unsigned int i;
  
  for(i = 0; i < length; i++)
  {
   result* r = &(rv->results[pos + i]);
   

    
    char c = seq[i];
       switch (c)
      {
      case 'A':
      case 'a':
	   r->coverage ++;
	r->A++;
	break;
      case 'C':
      case 'c':
	   r->coverage ++;
	r->C++;
	break;
      case 'G':
      case 'g':
	   r->coverage ++;
	r->G++;
	break;
      case 'T':
      case 't':
	   r->coverage ++;
	r->T++;
	break;
      case 'N':
      case 'X':
      case '-':
	   r->coverage ++;
	r->N++;
	break;
      default:
	printf("seg ='%s'",seq);
	assert(0);//A sequence should not contain anything but DNA characters... 
		  // TODO if this is a limiting factor, simply skip the entry but keep i small
		  // Also, make sure the same is true for qualities...
        break;
      }
  }
  
  
  
}

void revCompReadToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length)
{
  
  unsigned int i;
  
  for(i = 0; i < length; i++)
  {
   result* r = &(rv->results[pos + i]);
   

    
    char c = seq[length -1 -i];
       switch (c)
      {
      case 'T':
      case 't':
	   r->coverage ++;
	r->A++;
	break;
      case 'G':
      case 'g':
	   r->coverage ++;
	r->C++;
	break;
      case 'C':
      case 'c':
	   r->coverage ++;
	r->G++;
	break;
      case 'A':
      case 'a':
	   r->coverage ++;
	r->T++;
	break;
      case 'N':
      case 'X':
      case '-':
	   r->coverage ++;
	r->N++;
	break;
      default:
	printf("seg ='%s'",seq);
	assert(0);//A sequence should not contain anything but DNA characters... 
		  // TODO if this is a limiting factor, simply skip the entry but keep i small
		  // Also, make sure the same is true for qualities...
        break;
      }
  }
  
  
  
}
void qualityToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length)
{
  
  unsigned int i,j;
  
  unsigned int value;
  
  result *r;
  for(i = 0; i < length; i++)
  {
    if((int)seq[i] < 33)
    {
      printf("%s\n",seq);
      printf("seq[%u]='%c'\n",i, seq[i]);
    assert((int)seq[i] >= 33);
    }
  value = (int)seq[i] - 33;
  
  r=&(rv->results[pos + i]);
  

  if(value + 1 > r->indivErrorLength)
  {
    r->indivError = realloc(r->indivError, (value + 1) * sizeof(unsigned int));
    for(j = r->indivErrorLength; j < (value + 1); j++)
    {
      r->indivError[j] = 0;
    }
    r->indivErrorLength = (value +1);
  }
  r->indivError[value]++;
  
  }
  
  
  
}

void revQualityToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length)
{
  
  unsigned int i,j;
  
  unsigned int value;
  
  result *r;
  for(i = 0; i < length; i++)
  {
    if((int)seq[length -1 -i] < 33)
    {
      printf("%s\n",seq);
      printf("seq[%u]='%c'\n",i, seq[length -1 -i]);
    assert((int)seq[length -1 -i] >= 33);
    }
  value = (int)seq[length -1 -i] - 33;
  
  r=&(rv->results[pos + i]);
  

  if(value + 1 > r->indivErrorLength)
  {
    r->indivError = realloc(r->indivError, (value + 1) * sizeof(unsigned int));
    for(j = r->indivErrorLength; j < (value + 1); j++)
    {
      r->indivError[j] = 0;
    }
    r->indivErrorLength = (value +1);
  }
  r->indivError[value]++;
  
  }
  
  
  
}

void postProcessResults(setting arg, resultsVector *rv)
{
  unsigned int i, j, k, sum, count;
  int median, uQuartile, lQuartile;
  int lower5, upper95;
  double fmedian, fsum;
  int max = -1;
  int min = -1;
  unsigned int second;
  char majorBase;
  result *r;
  unsigned int *e;
  
  double sumPerSiteCoverage       = 0;
  double sumQFloorPerSiteCoverage = 0;

	for(i = 0; i < rv->assignedLength; i++)
	{
	  r = &(rv->results[i]);
	  
	  assert( (r->A + r->C + r->G + r->T + r->N) == r->coverage);
  //Find major base and second basefrequency
	  
	  
	  //A
	  max = r->A;
	  majorBase = 'A';
	  
	//C
	 if(max < r->C) 
	 {
	 second = max;
	 max = r->C;
	 majorBase = 'C';
	 }else{
	  second = r->C; 
	 }
	 
	 
	//G
	 if(max < r->G) 
	 {
	 second = max;
	 max = r->G;
	 majorBase = 'G';
	 }else{
	   if(r->G > second)
	  second = r->G; 
	 }
	   
	  
	//T
	 if(max < r->T) 
	 {
	 second = max;
	 max = r->T;
	 majorBase = 'T';
	 }else{
	   if(r->T > second)
	  second = r->T; 
	 }	  
	  
	//N
	 if(max < r->N) 
	 {
	 second = max;
	 max = r->N;
	 majorBase = 'N';
	 }else{
	   if(r->N > second)
	  second = r->N; 
	 }
	 
	 r->majorBase = majorBase;
	 r->majorBaseCount = max;
	 r->secondBaseCount = second;
	 
	 r->frequencyMajorBase  = (double)max/r->coverage;
	 r->frequencySecondBase = (double)second/r->coverage;
	 
// Find Quality scores
	 min  = -1;
	 max  = -1;
	 sum  =  0;
	 fsum =  0;
	 count=  0;
	 median  = -1;
	 lQuartile  = -1;
	 uQuartile  = -1;
	 lower5 = -1;
	 upper95= -1;
	 
	 fmedian = -1;
	 
	 r->qFloorCoverage = 0;

//	   for(j=0; j < r->indivErrorLength; j++)
//	   {
//	     e = r->indivError;
//	    printf("%u ", e[j]); 
//	   }
//	   printf("\n");
	   for(j=0; j < r->indivErrorLength; j++)
	   {
	     e = r->indivError;//NOTE this is the number of times a score of j has been observed!
	     sum   += j * e[j];
	     fsum  += Q2P(j)  * e[j];
	     count += e[j];
//coverage above the quality threhold	     
	     if(arg.qFloor > 0 && arg.qFloor == j+1)
	     {
	       r->qFloorCoverage = r->coverage - count;
	     }
//Median	     
	     if(count >= (double)r->coverage/2 && median < 0)
	     {
	       if(count > (double)r->coverage/2)
	       {
		 median = j;
		fmedian = Q2P(j);
	       }else{
		 
		 for(k=j+1; k< r->indivErrorLength ;k++)
		 {
		   if(e[k] > 0)
		     break;
 
		 }
		median = floor( (j+k)/2 + 0.5); 
		fmedian= floor( (Q2P(j) + Q2P(k))/2     + 0.5);
	       }
	       
	     }	     
//25% lower quartile	     
	     	     if(count >= (double)r->coverage/4 && lQuartile < 0)
	     {
	       if(count > (double)r->coverage/4)
	       {
		 lQuartile = j;
		}else{
		 
		 for(k=j+1; k< r->indivErrorLength ;k++)
		 {
		   if(e[k] > 0)
		     break;
		 }
		lQuartile = floor( (j+k)/2 + 0.5); 
		}
	       
	     }	
	     
//75% upper quartile	     
	     	     if(count >= 3*(double)r->coverage/4 && uQuartile < 0)
	     {
	       if(count > 3*(double)r->coverage/4)
	       {
		 uQuartile = j;
		}else{
		 
		 for(k=j+1; k< r->indivErrorLength ;k++)
		 {
		   if(e[k] > 0)
		     break;
		 }
		uQuartile = floor( (j+k)/2 + 0.5); 
		}
	       
	     }	
	     
//95% upper 	     
	     	     if(count >= 0.95*r->coverage && upper95 < 0)
	     {
	       if(count > 0.95*r->coverage)
	       {
		 upper95 = j;
		}else{
		 
		 for(k=j+1; k< r->indivErrorLength ;k++)
		 {
		   if(e[k] > 0)
		     break;
		 }
		upper95 = floor( (j+k)/2 + 0.5); 
		}
	       
	     }	
	     
//5% lower 	     
	     	     if(count >= 0.05*r->coverage && lower5 < 0)
	     {
	       if(count > 0.05*r->coverage)
	       {
		 lower5 = j;
		}else{
		 
		 for(k=j+1; k< r->indivErrorLength ;k++)
		 {
		   if(e[k] > 0)
		     break;
		 }
		lower5 = floor( (j+k)/2 + 0.5); 
		}
	       
	     }		     
	     
	     
	     if(e[j] > 0)
	     {
	       max = j;
	     }
	     if(e[j] > 0 && min < 0)
	     {
	       min = j;
	     }
	     
 
	   }
	 
	// assert(sumPerSiteCoverage < ULLONG_MAX - r->coverage);	// If this assertion fail (which it may),
								// we have to code around it by changing the mean on the fly
	 sumPerSiteCoverage += r->coverage;
	 sumQFloorPerSiteCoverage += r->qFloorCoverage;
	 
	 r->meanPhError     = floor((double)sum/r->coverage + 0.5);
	 r->medianPhError   = median;
	 r->lowerPhQuartile = lQuartile;
         r->upperPhQuartile = uQuartile;
	// r->minPhError      = min;
	// r->maxPhError      = max;
	 r->minPhError      = lower5;
	 r->maxPhError      = upper95;
	 
	 r->meanError = fsum/r->coverage;
	 r->medianError = fmedian;
	 r->minError = Q2P(min);
	 r->maxError = Q2P(max);
	}
  
    rv->averageCoverage       = sumPerSiteCoverage/(double)rv->assignedLength;
    rv->averageQFloorCoverage = sumQFloorPerSiteCoverage/(double)rv->assignedLength;
}


void printCSV(FILE *file, resultsVector rv)
{
    unsigned int i;
    result r;

     fprintf(file, ",As,Cs,Gs,Ns,Ts,coverage,expected_number_of_errors,majorbase_ratio,majorbases,majorsequence,position,probability_of_seq_error,secondbase,secondbase_ratio, indels\n");
     
	for(i = 0; i < rv.assignedLength; i++)
	{
	  r = rv.results[i];
	  
	  
	fprintf(file, "%u, %u, %u, %u, %u, %u, %u, %f, %f, %u, %c, %u, %f, %u, %f, %u\n", 
	       i, r.A, r.C, r.G, r.N, r.T, r.coverage, r.coverage * r.meanError, r.frequencyMajorBase, r.majorBaseCount, r.majorBase, i, r.meanError, r.secondBaseCount, r.frequencySecondBase, r.numIndels );
	}
  
  
}

void printGnuplotDat(FILE* file, resultsVector rv)
{
  unsigned int i, sum;
  result r;
  fprintf(file, "#pos \tmin \tQ1 \tMedian \tQ3 \tmax \tMean \tCoverage \tqCoverage \tA \tC \tG \tT\n");
  	for(i = 0; i < rv.assignedLength; i++)
	{
	  r = rv.results[i];
	  
	  sum = r.A + r.C + r.G + r.T;
	 fprintf(file, "%u \t%u \t%u \t%u \t%u \t%u \t%u \t%u \t%u \t%f \t%f \t%f \t%f\n"
	               , i, r.minPhError, r.lowerPhQuartile, r.medianPhError, r.upperPhQuartile, r.maxPhError, r.meanPhError, r.coverage, r.qFloorCoverage, r.A/(double)sum, r.C/(double)sum, r.G/(double)sum, r.T/(double)sum); 
	  
	}
	fflush(file);
}



//--Plotting-Functions------------------------------------------------
void plotQualityProfile(setting arg, int length, char* tempFileName)
{
unsigned int howMany = arg.numPlots;
unsigned int every;


if(howMany < length)
every = floor((double)length / howMany);
else
every = 1;



FILE * gnuplotPipe = popen("gnuplot -persistent", "w");

fprintf(gnuplotPipe,"set bars 2.0\n");
fprintf(gnuplotPipe,"set style fill solid 0.25 border\n");
fprintf(gnuplotPipe,"set yrange[0:]\n");
fprintf(gnuplotPipe,"set boxwidth %.2f\n", (double)every*2.0/3.0);
fprintf(gnuplotPipe,"set grid ytics lc rgb \"#bbbbbb\" lw 1 lt 0\n");
if(!arg.concealResults)
{
//fprintf(gnuplotPipe, "set term wxt title 'Quality Profiles'\n");
fprintf(gnuplotPipe, "set term x11 title 'Quality Profiles'\n");
//fprintf(gnuplotPipe, "plot '%s' every %u using 1:3:2:6:5 with candlesticks title 'Range [0.05:0.95] and Quartiles' whiskerbars, '%s' every %u using 1:4:4:4:4 with candlesticks lt -1 title 'Median', '%s' every %u u 1:7 smooth csplines title 'Mean'\n"
//		   , tempFileName, every, tempFileName, every, tempFileName, every);
fprintf(gnuplotPipe, "plot '%s' every %u using 1:3:2:6:5 with candlesticks title 'Range [0.05:0.95] and Quartiles' whiskerbars, '%s' every %u using 1:4:4:4:4 with candlesticks lt -1 title 'Median', '%s' u 1:7 smooth csplines title 'Mean'\n"
		   , tempFileName, every, tempFileName, every, tempFileName);
fflush(gnuplotPipe);
}

if(arg.outFilePrefix)
{
char *tempPNGName;
tempPNGName = (char*)calloc(strlen(arg.outFilePrefix)+16, sizeof(char));
sprintf(tempPNGName, "%s.qualityProfile", arg.outFilePrefix);

fprintf(gnuplotPipe,"set title 'Quality Profiles'\n");
fprintf(gnuplotPipe,"set terminal png\n");
fprintf(gnuplotPipe,"set output '%s.png'\n", tempPNGName);
//fprintf(gnuplotPipe, "plot '%s' every %u using 1:3:2:6:5 with candlesticks title 'Range [0.05:0.95] and Quartiles' whiskerbars, '%s' every %u using 1:4:4:4:4 with candlesticks lt -1 title 'Median', '%s' every %u u 1:7 smooth csplines title 'Mean'\n"
//		   , tempFileName, every, tempFileName, every, tempFileName, every);
fprintf(gnuplotPipe, "plot '%s' every %u using 1:3:2:6:5 with candlesticks title 'Range [0.05:0.95] and Quartiles' whiskerbars, '%s' every %u using 1:4:4:4:4 with candlesticks lt -1 title 'Median', '%s' u 1:7 smooth csplines title 'Mean'\n"
		   , tempFileName, every, tempFileName, every, tempFileName);

fflush(gnuplotPipe);

printf("Quality profiles output as:\t \"%s.png\"\n",tempPNGName);
free(tempPNGName);  
}

pclose(gnuplotPipe);
  
}


void plotCoverage(setting arg, char* tempFileName)
{


FILE * gnuplotPipe = popen("gnuplot -persistent", "w");

fprintf(gnuplotPipe,"set yrange[0:]\n");

if(!arg.concealResults)
{
fprintf(gnuplotPipe, "set term x11 title 'Coverage of Sites'\n");
fprintf(gnuplotPipe, "plot '%s' using 1:8 with lines title 'Coverage'\n", tempFileName);
if(arg.qFloor >= 0){
fprintf(gnuplotPipe, "replot '%s' using 1:9 with lines title 'Quality limited Coverage Q >= %u'\n", tempFileName, arg.qFloor);  
}
fflush(gnuplotPipe);
}

if(arg.outFilePrefix)
{
char *tempPNGName;
tempPNGName = (char*)calloc(strlen(arg.outFilePrefix)+17, sizeof(char));
sprintf(tempPNGName, "%s.coverageProfile", arg.outFilePrefix);

fprintf(gnuplotPipe,"set title 'Coverage of Sites'\n");
fprintf(gnuplotPipe,"set terminal png\n");
fprintf(gnuplotPipe,"set output '%s.png'\n", tempPNGName);
if(arg.qFloor < 0){
fprintf(gnuplotPipe, "plot '%s' using 1:8 with lines title 'Coverage'\n", tempFileName);
}else{
fprintf(gnuplotPipe, "plot '%s' using 1:8 with lines title 'Coverage', '%s' using 1:9 with lines title 'Quality limited Coverage Q >= %u'\n", tempFileName, tempFileName, arg.qFloor);  
}
fflush(gnuplotPipe);

printf("Coverage profile output as:\t \"%s.png\"\n",tempPNGName);
free(tempPNGName);  
}


pclose(gnuplotPipe);
  
}




void plotBaseFrequencies(setting arg, char* tempFileName)
{


FILE * gnuplotPipe = popen("gnuplot -persistent", "w");

fprintf(gnuplotPipe,"set yrange[0:]\n");

if(!arg.concealResults)
{
fprintf(gnuplotPipe, "set term x11 title 'Relative Base Frequencies'\n");
fprintf(gnuplotPipe, "plot '%s' using 1:10 with lines title 'A'\n", tempFileName);
fprintf(gnuplotPipe, "replot '%s' using 1:11 with lines title 'C'\n", tempFileName);
fprintf(gnuplotPipe, "replot '%s' using 1:12 with lines title 'G'\n", tempFileName);
fprintf(gnuplotPipe, "replot '%s' using 1:13 with lines title 'T'\n", tempFileName);
fflush(gnuplotPipe);
}

if(arg.outFilePrefix)
{
char *tempPNGName;
tempPNGName = (char*)calloc(strlen(arg.outFilePrefix)+22, sizeof(char));
sprintf(tempPNGName, "%s.baseFrequencyProfile", arg.outFilePrefix);

fprintf(gnuplotPipe,"set title 'Relative Base Frequencies'\n");
fprintf(gnuplotPipe,"set terminal png\n");
//fprintf(gnuplotPipe,"set term pngcairo\n");  //NOTE this produces nicer results but is less portable...
fprintf(gnuplotPipe,"set output '%s.png'\n", tempPNGName);

fprintf(gnuplotPipe, "plot '%s' using 1:10 with lines title 'A', '%s' using 1:11 with lines title 'C',  '%s' using 1:12 with lines title 'G', '%s' using 1:13 with lines title 'T'\n", tempFileName, tempFileName, tempFileName, tempFileName);
fflush(gnuplotPipe);

printf("Base frequencies output as:\t \"%s.png\"\n",tempPNGName);
free(tempPNGName);  
}


pclose(gnuplotPipe);
  
}


void plotMajorBase(setting arg, char* tempFileName)
{


FILE * gnuplotPipe = popen("gnuplot -persistent", "w");

fprintf(gnuplotPipe,"set yrange[0.86:]\n");

if(!arg.concealResults)
{
fprintf(gnuplotPipe, "set term x11 title 'Relative Base Frequencies'\n");
fprintf(gnuplotPipe, "plot '%s' using 1:($10) with dots title 'A'\n", tempFileName);
fprintf(gnuplotPipe, "replot '%s' using 1:($11) with dots title 'C'\n", tempFileName);
fprintf(gnuplotPipe, "replot '%s' using 1:($12) with dots title 'G'\n", tempFileName);
fprintf(gnuplotPipe, "replot '%s' using 1:($13) with dots title 'T'\n", tempFileName);
fflush(gnuplotPipe);
}

if(arg.outFilePrefix)
{
char *tempPNGName;
tempPNGName = (char*)calloc(strlen(arg.outFilePrefix)+22, sizeof(char));
sprintf(tempPNGName, "%s.baseFrequencyProfile", arg.outFilePrefix);

fprintf(gnuplotPipe,"set title 'Relative Base Frequencies'\n");
fprintf(gnuplotPipe,"set terminal png\n");
//fprintf(gnuplotPipe,"set term pngcairo\n");  //NOTE this produces nicer results but is less portable...
fprintf(gnuplotPipe,"set output '%s.png'\n", tempPNGName);

fprintf(gnuplotPipe, "plot '%s' using 1:($10) with dots title 'A', '%s' using 1:($11) with dots title 'C',  '%s' using 1:($12) with dots title 'G', '%s' using 1:($13) with dots title 'T'\n", tempFileName, tempFileName, tempFileName, tempFileName);
fflush(gnuplotPipe);

printf("Base frequencies output as:\t \"%s.png\"\n",tempPNGName);
free(tempPNGName);  
}


pclose(gnuplotPipe);
  
}


void printAvgCoverage(setting arg, resultsVector rv)
{
int l, diff = 0;
if(arg.qFloor > 0)
{
  if(rv.averageQFloorCoverage == 0)
diff = floor(log(rv.averageCoverage)/log(10) + 1) - ( 1 + floor(log(arg.qFloor)/log(10) + 1) );
else
  diff = floor(log(rv.averageCoverage)/log(10) + 1) - ( floor(log(rv.averageQFloorCoverage)/log(10) + 1) + floor(log(arg.qFloor)/log(10) + 1) );
}
//printf("avgC %f, avgQC %f, qfllo %d diff %d\n", rv.averageCoverage, rv.averageQFloorCoverage, arg.qFloor, diff);
//exit(1);

printf("\nAverage coverage per site:    ");

	for(l = 0; l > diff; l--)
	printf(" ");

printf("%.1f\n", rv.averageCoverage);

if(arg.qFloor > 0)
{
  printf("Average coverage with Q >= %d:  ", arg.qFloor);

	for(l = 0; l < diff; l++)
	printf(" ");

printf("%.1f\n", rv.averageQFloorCoverage);
}
printf("\n");
}