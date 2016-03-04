#ifndef _SIM_QC   /* Include guard */
#define _SIM_QC

#include <stdio.h>

//#define _hashTableSize 10
//#define _hashTableSize 1327
//#define _hashTableSize 11777
//#define _hashTableSize   1002017 
//#define _hashTableSize 1252159
#define _hashTableSize 100003

#define _hashValue 37
//#define _hashValue 1000861


//#define _singleUnsigned
#define _multiUnsigned
#define _numMulti 1


//setup for A==0001, C==0010, G==0100, T==1000
#define basesPerByte 2
#define bitsPerBase 4

//setup for A==00, C==01, G==10, T==11, DOES NOT ACCOUNT FOR DEGENERATE BASES!
//#define basesPerByte 4
//#define bitsPerBase 2

//#define _32BIT
#ifdef _32BIT
FILE* fopen64(const char *filename, const char *type);
#endif

typedef struct
{
unsigned int verbose;

char* referenceFile;
char* readsFile;

char* dataInFile;
char* outFilePrefix;
int qFloor;

unsigned int concealResults;

unsigned int numPlots;

unsigned int primes;

unsigned int windows;
}setting;


#ifdef _singleUnsigned
typedef struct
{
unsigned int value;
unsigned int repeats;

unsigned int *hitList;
unsigned int allocatedHitList;
unsigned int inHitList;

int next;

}hashEntry;
#endif
#ifdef _multiUnsigned
typedef struct
{
unsigned int *value;
unsigned int repeats;

unsigned int *hitList;
unsigned int allocatedHitList;
unsigned int inHitList;

int next;

}hashEntry;
#endif




typedef struct
{
  char * referenceSequence;
  unsigned int referenceSequenceLength; 
  

 //hash table of POINTERS! for entries.
 hashEntry** hashTable;
 //Table holding the actual entrier.
 hashEntry* entryTable;
 unsigned int itemsInTable;
 
 //hash table of POINTERS! for entries of reverse complemented string.
 hashEntry** complementHashTable;
 //Table holding the actual entrier.
 hashEntry* complementEntryTable;
 unsigned int itemsInComplementTable;
 
}globalVariables;


typedef struct
{
  unsigned int A;
  unsigned int C;
  unsigned int G;
  unsigned int T;
  unsigned int N;
  
  unsigned int coverage;
  unsigned int qFloorCoverage;
  
  unsigned int * indivError;

  unsigned int indivErrorLength;
  
  char majorBase;
  unsigned int majorBaseCount;
  unsigned int secondBaseCount;
  double frequencyMajorBase;
  double frequencySecondBase;
  
  unsigned int meanPhError;
  unsigned int medianPhError;
  unsigned int lowerPhQuartile;
  unsigned int upperPhQuartile;
  unsigned int minPhError;
  unsigned int maxPhError;
  
  double meanError;
  double medianError;
  double minError;
  double maxError;
  

}result;

typedef struct
{
  result * results;
   unsigned int assignedLength;
   
   double averageCoverage;
   double averageQFloorCoverage;
}resultsVector;

//int populateHashTableWithKey(setting * arg, unsigned int hashValue, unsigned int hashTableSize);


#endif
