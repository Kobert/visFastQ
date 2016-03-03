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
#include "visQ_arg.h"


  void simQC_help()
 {
  printf("\n");
  printf("version 0.1.0\n");   
  printf("\n"); 
  printf("Important: In order to use the program, you must have gnuplot\n");
  printf("           (preferebly gnuplot-x11) installed!\n"); 
  
  printf("\n"); 
  printf("Required arguments are:\n"); 
  printf("\n"); 
  
  printf("One of the following input files:\n");     
  
  printf("\t-s for specifying a file with shortreads (fastq format only).\n");     
  printf("\t-d for specifying a previously obtained data file.\n");     
 
  printf("\nOptional arguments:\n");
  printf("The output:\n");       
  printf("\t-o specify a prefix for the output names (can include a path).\n");  
  printf("\t   If no Prefix is provided, no *.png files will be written.\n");
  printf("\t   If the arguement is \"-o SOMEPATH/SOMENAME\", the following\n");
  printf("\t   files will be written to the SOMEPATH/ folder:\n");
  printf("\t\t   SOMENAME.dataPoints.dat\n");
  printf("\t\t   SOMENAME.qualityProfile.png\n");
  printf("\t\t   SOMENAME.coverageProfile.png\n");
  printf("\t\t   SOMENAME.baseFrequencyProfile.png\n");
  printf("\n"); 
  printf("-m Specify how many boxes to utilise for the quality profile boxplot.\n");
  printf("   (The actual number may be +-1) (default 40) \n");
  printf("-c Conceals output. Does not open a gnuplot plot window. \n");
  printf("   An alternative output must be set with -o instead. \n");
  
  
  printf("-h print this help message.\n");    
  printf("\n");
 }
 
 
 void simQC_freeSetting(setting *s)
 {
   if(s->readsFile)
   free(s->readsFile);
   
   if(s->outFilePrefix)
   free(s->outFilePrefix);
   
   if(s->dataInFile)
     free(s->dataInFile);
 }
 
 setting simQC_initArgs(int argc, char **argv)
 {
  setting s;
  
  s.verbose = 1;
  s.numPlots = 40;
  
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
  
  s.outFilePrefix = NULL;
  
  s.dataInFile = NULL;

  s.concealResults = 0;
  
  opterr = 0;

  while ((c = getopt (argc, argv, "d:ho:s:m:c")) != -1)
    switch (c)
      {
      case 'h':
	simQC_help();
	exit(1);
	break;
      case 'd':
         s.dataInFile = strdup(optarg);
        break;
      case 's':
        s.readsFile = strdup(optarg);
        break;
      case 'o':
        s.outFilePrefix = strdup(optarg);
        break;	
      case 'm':
        s.numPlots = atoi(optarg);
        break;
      case 'c':
        s.concealResults = 1;
        break;
      case '?':
        if (optopt == 's' || optopt == 'm' || optopt == 'd' || optopt == 'o' )
	{
	  simQC_help();
          fprintf (stderr, "[ERROR:] Option -%c requires an argument.\n", optopt);
	  assert(0);
	}
	else if (isprint (optopt))
	{
	  simQC_help();
          fprintf (stderr, "[ERROR:] Unknown option `-%c'.\n", optopt);
	
	  assert(0);
	}
	else
	{
	  simQC_help();
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
	}
        return s;
      default:
        abort ();
      }

      
	for (index = optind; index < argc; index++)
	printf ("Non-option argument %s\n", argv[index]);

	
	
	if(!s.readsFile && !s.dataInFile)
	{   
	  simQC_help();
	 printf("[ERROR:] Please specify an input file with -s or -d command\n");
	assert(s.readsFile);  
	}
	if(s.readsFile && s.dataInFile)
	{
	  simQC_help();
	 printf("[WARNING:] Please only specify a data file to plot, OR a fastq file containing short reads.\n");
	assert(0);  
	  
	}
	if(!s.outFilePrefix && s.concealResults)
	{
	 simQC_help();
	 printf("[WARNING:] You are supressing (i.e. concealing) the results from being printed to a window by setting -c.\n");
	 printf("           Please specify an alternative output via -o .\n");
	assert(0);  
	  
	}

 
  return s;
 }
