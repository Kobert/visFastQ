#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
 
#include "referenceAssembly.h"
#include "ref_arg.h"
#include "ref_hash.h"
#include "ref_slidingWindow.h"
 
 
 
 void help()
 {
  printf("\n"); 
  printf("Function arguments are:\n"); 
  printf("\n"); 
  printf("-r for specifying a file containing the reference sequence\n");    
  printf("-s for specifying a file with shortreads (fastq format only)\n");    
  printf("-h print this help messgae\n");    
  printf("\n\n");
  printf("Developers Options:\n");    
  printf("-d for more verbose debug output\n"); 
  printf("-p for testing primenumber configurations (no reads file from -s is needed)\n");   
  printf("\n");
 }
 
 
void freeSetting(setting s)
{
  if(s.referenceFile)
    free(s.referenceFile);
  
  if(s.readsFile)
    free(s.readsFile);
  
}
 
 setting initArgs(int argc, char **argv)
 {
  setting s;
  
  s.verbose = 1;
  
  int index;
  int c;
  s.primes  = 0;

#ifdef _multiUnsigned
  s.windows = _numMulti;
#else
  s.windows = 1;
#endif
  
  s.referenceFile = NULL;
  
  s.readsFile = NULL;

  opterr = 0;

  while ((c = getopt (argc, argv, "dhr:s:pw:")) != -1)
    switch (c)
      {
      case 'h':
	help();
	exit(1);
	break;
      case 'p'://Check behaviour of different prime numbers
	s.primes = 1;
        break;      
      case 'r':
	s.referenceFile = strdup(optarg);//printf("%s\n", optarg);
        break;
      case 'd':
        s.verbose = 2;
        break;
      case 's':
        s.readsFile = strdup(optarg);
        break;
      case 'w':
        s.windows = atoi(optarg);
        break;
      case '?':
        if (optopt == 's' || optopt == 'r' || optopt == 'w' )
	{
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
	  assert(0);
	}
	else if (isprint (optopt))
	{
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
	
	  assert(0);
	}
	else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return s;
      default:
        abort ();
      }

      
	for (index = optind; index < argc; index++)
	printf ("Non-option argument %s\n", argv[index]);

	if(!s.referenceFile)
	{
	printf("Please specify reference file with -r command\n");
	assert(s.referenceFile);
	}
	
	if(!s.readsFile && !s.primes)
	{   
	 printf("Please specify reads file with -s command\n");
	assert(s.readsFile);  
	}
  
 
  return s;
 }
 
 
 void freeGlobalVariables(globalVariables *globalVar)
 {
   
  free(globalVar->referenceSequence); 
  
  free(globalVar->hashTable);
  
  freeTable(globalVar->entryTable, globalVar->itemsInTable);
  free(globalVar->entryTable);
  
  free(globalVar->complementHashTable);
  
  freeTable(globalVar->complementEntryTable, globalVar->itemsInComplementTable);
  free(globalVar->complementEntryTable);  

 }
 
 //-------------Read-Reference-File---------------------------------------------------------------
 //Will attempt to read arg->referenceFile and write its contents to globalVar->referenceSequence
 void readReferenceFile(setting *arg, globalVariables *globalVar)
 {
   

     FILE *referenceFile;
     referenceFile = fopen(arg->referenceFile,"r");
	
        //Check that file opened succesfully
	if(!referenceFile)
	{
	printf("\n[ERROR:] Could not open file %s\n", arg->referenceFile);
	  printf("         Please make sure the path is correct and you have read permission.\n");
	assert(referenceFile);
	}
      
  unsigned int verbose = arg->verbose;    
  
  unsigned int i;
      
  int ic;
  char c;
  
  unsigned int foundName = 0;
  unsigned int determinedLF = 0;
  unsigned int determinedCR = 0;  

  int seqLength;
  unsigned int bytesToRead = 10;
  char * seq;
  seq = (char*)malloc(bytesToRead);
  
  seqLength = getline(&seq, &bytesToRead,referenceFile);
  assert(seqLength >= 0);
  
  unsigned int sumLengths = 0;
  seqLength = getline(&seq, &bytesToRead,referenceFile);
   
	while( seqLength > 0)
	{
	sumLengths += seqLength;
	seqLength = getline(&seq, &bytesToRead,referenceFile);
	}
   
   rewind(referenceFile);
   
   //Allocate initial space for reference sequence.
   // Space allocated here will in almost all cases be strictly more than needed.
   globalVar->referenceSequence = (char*)malloc((sumLengths+1) * sizeof(char));
   
   
        //Check to see whether we find a sequence name starting with either > or @
        while(!foundName)
	{
	assert(ic = fgetc(referenceFile));
	c = (char)ic;
		if(ic == EOF)
		{
		printf("[ERROR:] Could not find beginning of sequence in %s\n", arg->referenceFile);
		assert(ic != EOF);
		}

		 if(c == '>' || c == '@')
		foundName=1;
	}
  
  
        //The getline() command used below does not support linebreaks that are denoted only by \r
        // (instead of \n or \r\n). So make sure we have a readable format.
	while(!determinedLF)
	{
	assert(ic = fgetc(referenceFile));
	c = (char)ic;
		if(ic == EOF)
		{
		printf("[ERROR:] Could not find line feed character \\n in %s\n", arg->referenceFile);
			if(determinedCR)
			{
			printf("         However, CR \\r was found. At present this software relies on \\n though.\n");
			printf("         You may try converting the format with tools such as mac2unix of the dos2unix package.\n");
			}
		assert(ic != EOF);
		}

		if(c == '\n' )
		determinedLF = 1;
		if(c == '\r')
		determinedCR = 1;  
	}
	
	
	
  seqLength = getline(&seq, &bytesToRead,referenceFile);
  sumLengths = 0;
  while(seqLength>=0)
{

  if(verbose > 1)
  {
  printf("%s",seq);
  }
	for(i=0;i<strlen(seq);i++)//Go through the whole sequence and simply write down the appropriate bases.
	{
	  if( mapUnsafe(seq[i]) < 0)
	  continue;
	  {
	globalVar->referenceSequence[sumLengths] = seq[i];
	
	sumLengths++;
	  }
	}
	  fflush(stdout);
	 seqLength = getline(&seq, &bytesToRead,referenceFile);
}
   
   if(verbose > 1)
   printf("\nSumLengths = %u\n", sumLengths);
   
   globalVar->referenceSequence = (char*)realloc(globalVar->referenceSequence, (sumLengths+1)*sizeof(char));
   globalVar->referenceSequenceLength = sumLengths;
   
   fclose(referenceFile);
  
   free(seq);

   
 }
    //------------------------------------------------------------------------------------
    
void initGlobalVariables(setting *arg, globalVariables *globalVar)
{
       
 readReferenceFile(arg, globalVar);      
 
  //allocate initial hash table of POINTERS! for entries.
  globalVar->hashTable = (hashEntry**)calloc(1, sizeof(hashEntry*));
  
  globalVar->entryTable = (hashEntry*)calloc(globalVar->referenceSequenceLength, sizeof(hashEntry)); 
  
  globalVar->itemsInTable = 0;
  
  //allocate initial hash table of POINTERS! for entries of complement sequence.
  globalVar->complementHashTable = (hashEntry**)calloc(1, sizeof(hashEntry*));
  
  globalVar->complementEntryTable = (hashEntry*)calloc(globalVar->referenceSequenceLength, sizeof(hashEntry)); 
  
  globalVar->itemsInComplementTable = 0;
  
  printf("\nSetting up hashtable for reference sequence...");
  populateHashTable(arg, globalVar, globalVar->referenceSequence, &(globalVar->hashTable), &(globalVar->entryTable), &(globalVar->itemsInTable));
  
  char * reverseComplement;
  reverseComplement = (char*)calloc(globalVar->referenceSequenceLength, sizeof(char));
  
  reverseComplementSequence(reverseComplement, globalVar->referenceSequence);
    
  printf("\nSetting up hashtable for reverse complemented reference sequence...");
  populateHashTable(arg, globalVar, reverseComplement, &(globalVar->complementHashTable), &(globalVar->complementEntryTable), &(globalVar->itemsInComplementTable));
  
  free(reverseComplement);
  
  
  printf("\nSetup completed!\n");
}



void freeResults(resultsVector *rv)
{
    unsigned int i;
    
    result* r;
//    printf("rv->assignedLength  in free %u\n",rv->assignedLength);
    for(i = 0; i<rv->assignedLength; i++)
    {
      r = &(rv->results[i]);
      free(r->indivError);
      //free( (&(rv->results[i]))->indivError);
    }
  
  free(rv->results);
}

void reallocResults(resultsVector *rv, unsigned int length)
{
  unsigned int i;
  
  rv->results = realloc(rv->results, length * sizeof(result));
	     
  result* r;
		for(i = rv->assignedLength; i < length; i++)
		{
		r = &(rv->results[i]);
		*r = (const result){ 0 };
		r->indivErrorLength = 41;
		r->indivError = (unsigned int*)calloc(r->indivErrorLength, sizeof(unsigned int));
		}
    
	     rv->assignedLength = length;
  
//  printf("rv->assignedLength  in realloc %u\n",rv->assignedLength);
  
}

void initResults(resultsVector *rv, unsigned int length)
{
  unsigned int i;
  
    rv->results =(result*)calloc(length, sizeof(result));
    rv->assignedLength = length;
    
    result *r;
    for(i = 0; i<length; i++)
    {
      r = &(rv->results[i]);
      r->indivErrorLength = 41;
      r->indivError = (unsigned int*)calloc(r->indivErrorLength, sizeof(unsigned int));
    }
  
}
