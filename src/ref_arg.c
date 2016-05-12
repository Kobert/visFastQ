#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "referenceAssembly.h"
#include "ref_arg.h"
#include "ref_hash.h"
#include "ref_slidingWindow.h"
 
 
 
 void help()
 {
  printf("\n"); 
  printf("Function arguments are:\n"); 
  printf("\n"); 
  printf(" -r for specifying a file containing the reference sequence (fasta format only)\n");    
  printf(" -s for specifying a file with shortreads (fastq format only)\n");    
  printf("\n");  
  printf("\n");    
  printf(" -o specify a prefix for the output names (can include a path).\n");  
  printf("    If no Prefix is provided, only generic temporary files will be written.\n");
  printf("\n");    
  printf(" -c Print results in form of a *.csv file.\n");
  printf("    A path should be provided with -o for this option.\n");    
  printf(" -g Plot results in Gnuplot.\n");
  printf("    A path can be provided with -o for this option.\n");
  printf("    If no path is provided, no *.png files will be written.\n");     
  printf("\n");  
  printf("\n");    
  printf(" -q Specify a lower bound for the quality scores (integer valued Phred score). \n");  
  printf("    Any base with a lower score will not count towards the coverage. \n");  
  printf("    This option only affects the coverage, all other plots remain the same! \n");    
  printf(" -h print this help messgae\n");    
  printf("\n\n");
  printf("Developers Options:\n");    
  printf(" -d for more verbose debug output\n"); 
  printf(" -f Specify how many fractions must match at least. (default 4, must be 2 or bigger)\n"); 
  printf(" -z for testing primenumber configurations (no reads file from -s is needed)\n");   
  printf("\n");
 }
 

void freeSetting(setting *s)
{
  if(s->referenceFile)
  {    
    free(s->referenceFile);
  }
  if(s->readsFile)
  {
    free(s->readsFile);
  }
    if(s->outFilePrefix)
  {
    free(s->outFilePrefix);
  }
}
 
 setting initArgs(int argc, char **argv)
 {
  setting s;
  
  s.verbose = 1;
  
  int index;
  int c;
  
  s.mapOnly = 0;
  s.primes  = 0;
  
  s.storeReads = 1;

#ifdef _multiUnsigned
  s.windows = _numMulti;
#else
  s.windows = 1;
#endif
  
  s.referenceFile = NULL;
  
  s.readsFile = NULL;

  s.outFilePrefix = NULL;
  
  s.doCsvFile = 0;
  s.doGnuplotFile = 0;
  
  s.numPlots = 40;
  
  s.minFracs = 4;
  //s.pValue = -1.0;
  
  s.doTrim = 0;
  s.doExtendedTesting = 0;
  
  opterr = 0;
  
  s.qFloor = -1;


  while ((c = getopt (argc, argv, "cdef:ghmr:s:tpw:o:q:z")) != -1)
    switch (c)
      {
      case 'h':
	help();
	exit(1);
	break;
      case 'c':
	s.doCsvFile = 1;
	break;	
      case 'e':
	s.doExtendedTesting = 1;
	break;		
      case 'f':
	assert(atoi(optarg) > 1);
	s.minFracs = atoi(optarg);
	break;		
      case 'g':
	s.doGnuplotFile = 1;
	break;
      case 'm':
	s.mapOnly = 1;
	s.storeReads = 0;
	break;
      case 'z'://Check behaviour of different prime numbers
	s.primes = 1;
        break;      
//     case 'p':
//	s.pValue = atoi(optarg);//printf("%s\n", optarg);
//        break;
      case 'r':
	s.referenceFile = strdup(optarg);//printf("%s\n", optarg);
        break;
      case 'd':
        s.verbose = 2;
        break;
      case 's':
        s.readsFile = strdup(optarg);
        break;
      case 't':
        s.doTrim = 1;
        break;
      case 'o':
        s.outFilePrefix = strdup(optarg);
        break;	
      case 'q':
        s.qFloor = atoi(optarg);
        break;	
      case 'w':
        s.windows = atoi(optarg);
	printf("[ERROR:] Feature '-w' not live yet! Aborting analysis.");
	assert(0);
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
  

 
  if(s.doGnuplotFile)
  {
	FILE * gnuplotPipe = popen("gnuplot -persistent", "w");
	if(!pclose(gnuplotPipe))
	{

	}else{
	help();
	printf("\n[ERROR:] Could not execute gnuplot.\n");
	printf("         Please make sure gnuplot is installed,\n");
	printf("         (preferably gnuplot-x11) or specify another\n");
	printf("         output format.\n\n");
	assert(0);
	}
  }
 
    if(s.doGnuplotFile && !s.outFilePrefix)
  {
   printf("\n[Note:] Gnuplot output is demanded without specifying an output path (via -o).\n ");   
   printf("        Plots will be opened in a seperate window after completion of the program.\n ");
   printf("        No *.png files will be written.\n ");
  }
  
  
    if(s.doCsvFile && !s.outFilePrefix)
  {
   printf("\n[Warning:] csv output is set without specifying an output path (via -o).\n ");   
   printf("           A generict temporary file will be written.\n");
  }
  
    if(!s.doGnuplotFile && !s.doCsvFile)//TODO add any new output formats here
  {
   printf("\n[Note:] No output format specified.\n ");   
   printf("        Only basic statistics will be printed to the screen.\n ");
   printf("        To see a list of available options run the program with the -h flag.\n ");
  }  
  
  return s;
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
//unsigned int bytesToRead = 10;
  size_t bytesToRead = 10;
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
   globalVar->referenceSequence[sumLengths] = '\0';
   globalVar->referenceSequenceLength = sumLengths;
   
   fclose(referenceFile);
  
   free(seq);

   
 }
    //------------------------------------------------------------------------------------
 void freeGlobalVariables(globalVariables *globalVar)
 {
  unsigned int i;
  
  free(globalVar->referenceSequence); 
  
  free(globalVar->hashTable);
  
  freeTable(globalVar->entryTable, globalVar->itemsInTable);
  free(globalVar->entryTable);
  
  for(i=0; i<globalVar->numReads; i++)
  {
	Read *r = &(globalVar->read[i]);
	if(!r->matchesReference)
	{
	free(r->seq);
	}
  }
  free(globalVar->read);

 }
 
void initGlobalVariables(setting *arg, globalVariables *globalVar)
{
       
 readReferenceFile(arg, globalVar);      
 
  //allocate initial hash table of POINTERS! for entries.
  globalVar->hashTable = (hashEntry**)calloc(1, sizeof(hashEntry*));
  
  globalVar->entryTable = (hashEntry*)calloc(globalVar->referenceSequenceLength, sizeof(hashEntry)); 
  
  globalVar->itemsInTable = 0;
  
  //allocate initial hash table of POINTERS! for entries of complement sequence.
  //globalVar->complementHashTable = (hashEntry**)calloc(1, sizeof(hashEntry*));
  
  //globalVar->complementEntryTable = (hashEntry*)calloc(globalVar->referenceSequenceLength, sizeof(hashEntry)); 
  
  //globalVar->itemsInComplementTable = 0;
  
  time_t timer;
  char buffer[10];
  struct tm* tm_info;

  time(&timer);
  tm_info = localtime(&timer);

  strftime(buffer, 10, "%H:%M:%S", tm_info);
    
  printf("\n[%s] Setting up hashtable for reference sequence...\n", buffer);
  populateHashTable(arg, globalVar, globalVar->referenceSequence, &(globalVar->hashTable), &(globalVar->entryTable), &(globalVar->itemsInTable));
 //   printf("\nItems in table %d\n",globalVar->itemsInTable);
  
 //Actually set up reverse complemented sequence
  //char * reverseComplement;
  //reverseComplement = (char*)calloc(strlen(globalVar->referenceSequence) +1, sizeof(char));
  
  //reverseComplementSequence(reverseComplement, globalVar->referenceSequence);

  //reverseComplement[strlen(globalVar->referenceSequence)]='\0';//setting terminal character
  
  //printf("\nSetting up hashtable for reverse complemented reference sequence...");
  //populateHashTable(arg, globalVar, reverseComplement, &(globalVar->complementHashTable), &(globalVar->complementEntryTable), &(globalVar->itemsInComplementTable));
  
  //free(reverseComplement);
 
	// Set up variables to store individual reads
	globalVar->numReads = 0;
	globalVar->allocatedNumReads = 1024;
	globalVar->read = (Read*)calloc(globalVar->allocatedNumReads ,sizeof(Read));
	
	
   
 globalVar->trimmed = 0;
 globalVar->kept = 0;
 
 globalVar->avgRatioTrimmed = 0.0;
 globalVar->avgTrimmedOf = 0;
  
  time(&timer);
  tm_info = localtime(&timer);

  strftime(buffer, 10, "%H:%M:%S", tm_info);
  printf("\n[%s] Setup completed!\n\n", buffer);
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
      
      r->numIndels = 0;
    }
    
    rv->indels = 0;
    rv->numReferenceMatches = 0;
}





//-----------------------------------------------------------------------------------------------------

void cleanForExit(setting * s, globalVariables * g,  resultsVector * rv)
{
  
  freeResults(rv);
  
  freeGlobalVariables(g);
  
  freeSetting(s);
  
}
