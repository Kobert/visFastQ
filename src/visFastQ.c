
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <math.h>


#include "referenceAssembly.h" 
#include "ref_arg.h"			// Functions to interpret user input and initialize datastructures go here.
#include "ref_hash.h"			// All functions that manipulate or access the tashtable or entries go here.
#include "ref_io.h"
#include "visQ_arg.h"





int main(int argc, char **argv)
{
  unsigned int i;
  int l;
  
 setting arg = simQC_initArgs(argc, argv);
 
 if(arg.readsFile)
 {
 printf("\nReading \"%s\" ...\n", arg.readsFile);

 resultsVector rv;

 
initResults(&rv,1);
  




   FILE *readsFile;
  readsFile = fopen(arg.readsFile,"r");
  if(!readsFile){
  readsFile = fopen64(arg.readsFile,"r");
  }
  if(!readsFile){
    printf("\n[ERROR:] Could not open file %s\n", arg.readsFile);
      printf("         Please make sure the path is correct and you have read permission.\n");
      assert(readsFile);
  }
  
  
  FILE * gnuplotPipe = popen("gnuplot -persistent", "w");
if(!pclose(gnuplotPipe))
{

}else{
   printf("[ERROR:]Could not execute gnuplot\n");

  assert(0);
}


//unsigned int bytesToRead = 10;
  size_t bytesToRead = 10;
  char * seq;
  seq = (char*)malloc(bytesToRead );
  


  unsigned int numReads = 0;
  
  
	while(getNextNonEmptyLine(readsFile, &seq, &bytesToRead))
	{
	  if(!getNextNonEmptyLine(readsFile, &seq, &bytesToRead))
	    break;
	   numReads ++;
	   
	   if(strlen(seq) > rv.assignedLength)
	   { reallocResults(&rv, strlen(seq)); }
	  

	  readToResult(&rv, seq, 0, strlen(seq));


	  //Get next non trivial read. Following a DNA sequence we should get a +, possibly with a descriptor.
	  //Next next line will then be a qualityprofile.
	  if(getNextNonEmptyLine(readsFile, &seq, &bytesToRead))
	  {
	      if(getNextNonEmptyLine(readsFile, &seq, &bytesToRead))
	      {
	      qualityToResult(&rv, seq, 0, strlen(seq));
	      }else{
		break;
	      }
	  }else{
	   break; 
	  }

	}


  free(seq);
  fclose(readsFile);



unsigned int j, sum=0;

for(i = 0; i < rv.assignedLength; i++)
{
  sum=0;
  result r = rv.results[i];

  for(j=0; j < r.indivErrorLength; j++)
  {
    sum+=r.indivError[j];
  }
  if(sum != r.coverage)
  {
      printf("pos: %u  \t",i);
    for(j=0; j < r.indivErrorLength; j++)
  {
    printf("%u ",r.indivError[j]);
  }
  printf(" \tsum: %u (%u)\n",sum, r.coverage);
  }
    assert(sum == r.coverage);
  }




postProcessResults(arg, &rv);


int diff = 0;
if(arg.qFloor > 0)
{
diff = floor(log(rv.averageCoverage)/log(10) + 1) - ( floor(log(rv.averageQFloorCoverage)/log(10) + 1) + floor(log(arg.qFloor)/log(10) + 1) );
}
//printf("diff %d\n", diff);


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


char *tempFileName;
if(arg.outFilePrefix)
{
tempFileName = (char*)calloc(strlen(arg.outFilePrefix)+16, sizeof(char));
sprintf(tempFileName, "%s.dataPoints.dat", arg.outFilePrefix);
}else{
 tempFileName = (char*)calloc(26, sizeof(char));

 sprintf(tempFileName, "_TEMPORARY.dataPoints.dat");
}

FILE * tempFile = fopen(tempFileName,"w");

printGnuplotDat(tempFile, rv);

fclose(tempFile);

printf("\nDatafile for Gnuplot written to: \"%s\"\n",tempFileName);
if(!arg.outFilePrefix)
{
 printf("File \"%s\" will be overwritten on the next call to this program!\n", tempFileName); 
}




plotQualityProfile(arg, rv.assignedLength, tempFileName);

plotCoverage(arg, tempFileName);

plotBaseFrequencies(arg, tempFileName);

free(tempFileName);



freeResults(&rv);



 }else{//ending the if arg.readsFile. If no readsfile is provided, a *.dat file is expected.
 
FILE * dataFile = fopen(arg.dataInFile, "r");
if(!dataFile)
{
  printf("[ERROR:] Could not open datafile: \"%s\"\n", arg.dataInFile);
  assert(arg.dataInFile);
}
char* dummy;

unsigned int count=0;
//unsigned int b;
size_t b;
  b = 10;
  dummy = (char*)calloc(b, sizeof(char));

	while(getline(&dummy, &b,dataFile)>0)
	{  count++; }
	
free(dummy);
fclose(dataFile);



      plotQualityProfile(arg, count, arg.dataInFile);
      
      plotCoverage(arg, arg.dataInFile);
   
      plotBaseFrequencies(arg, arg.dataInFile);

 }
   
   simQC_freeSetting(&arg);
  
   printf("\n");
 return 0; 
}



