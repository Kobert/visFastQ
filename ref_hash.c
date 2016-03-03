//All functions that manipulate or access the hashtable or entries go here.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "referenceAssembly.h"
#include "ref_slidingWindow.h"
#include "ref_hash.h"

//------------UTILITIES-FOR-ENTRIES-IN-HASHTABLE-------------------------------------------------------------------------------------------






#ifdef _singleUnsigned
void initEntry(hashEntry* entry, unsigned int * val)
{
entry->value=*val;
entry->repeats=1;
entry->next=-1;
}
#endif
#ifdef _multiUnsigned
void initEntry(hashEntry* entry, unsigned int * val)
{

unsigned int i;

entry->inHitList = 0;
entry->allocatedHitList = 10; //TODO 10 happens to work for the given dataset. Do not forget to realloc!!!!!
entry->hitList = (unsigned int *)calloc(entry->allocatedHitList, sizeof(unsigned int));
entry->value = (unsigned int *)calloc(_numMulti, sizeof(unsigned int));

for(i=0;i<_numMulti;i++)
{
entry->value[i]=val[i];
}

entry->repeats=1;
entry->next=-1;

}
#endif
  
#ifdef _singleUnsigned
void freeTable(hashEntry* entryList, unsigned int length)	//Not a clean solution, function should simply not be called if singleUnsigned is set...
{

}  
#endif
#ifdef _multiUnsigned
void freeTable(hashEntry* entryList, unsigned int length)
{
  
unsigned int i;
for(i=0; i<length; i++)
{
  hashEntry entry = entryList[i];
  
  free(entry.hitList);
  free(entry.value);
}
  
}
#endif  
  
#ifdef _singleUnsigned 
void printEntry(hashEntry e)
{
printWindow(&(e.value));
printf("Repeated %u times\n",e.repeats);

}
#endif
#ifdef _multiUnsigned 
void printEntry(hashEntry e)
{
//printWindow(&(e.value));
printWindow(e.value);
printf("Repeated %u times\n",e.repeats);

}
#endif

#ifdef _singleUnsigned
unsigned int sEQe(unsigned int* s,hashEntry e)	//Function to check whether a slidingwindow s matches an entry from the hashtable.
{						//Does not do much right now, but is anticipated to grow more complex.

if(*s==e.value)
return 1;

return 0;
}
#endif
#ifdef _multiUnsigned
unsigned int sEQe(unsigned int* s,hashEntry e)	//Function to check whether a slidingwindow s matches an entry from the hashtable.
{						//Does not do much right now, but is anticipated to grow more complex.

unsigned int i;

for(i=0; i<_numMulti; i++)
{
  if(s[i] != e.value[i])
    return 0;
}
  
return 1;

  
}
#endif
//-----------------------------------------------------------------------------------------------------------------------------





//-----------HASHING-FUNCTIONS-------------------------------------------------------------------------------------------------
#ifdef _singleUnsigned
unsigned int hashUI(unsigned int *value, unsigned int hashValue, unsigned int hashTableSize)	//Simple hashing function to hash unsigned integer values to some more managable range.
												//Must be changed to something more complex later...
{
  
unsigned long result = (((*value)*hashValue) % hashTableSize); //Use long unsigned int here to avoid problem with integer limits
return (unsigned int)result;
}
#endif
#ifdef _multiUnsigned
unsigned int hashUI(unsigned int *value, unsigned int hashValue, unsigned int hashTableSize)	//Simple hashing function to hash unsigned integer values to some more managable range.
{						//Must be changed to something more complex later...

unsigned int i;
unsigned long result=0;			//Use long unsigned int here to avoid problem with integer limits
for(i=0;i<_numMulti;i++)
{
result += value[i]*hashValue; 
}

return (unsigned int)(result % hashTableSize);

}
#endif



//If the hash position is not assigned yet, simply add the current element to it with default starting values.
void addNewToHashtable(unsigned int pos, unsigned int *slidingWindow, unsigned int sequencePosition, hashEntry **hashTable,  hashEntry *entryTable, unsigned int *itemsInTable)
{
 		hashEntry e;
		initEntry(&e,slidingWindow);
		
		e.hitList[0] = sequencePosition;
		e.inHitList++;
		entryTable[*itemsInTable]=e;
		hashTable[pos]=&entryTable[*itemsInTable];
		(*itemsInTable)++; 

}


//Check if there are chained elements. If none match, add a new entry to the list.
void chainHash(unsigned int *slidingWindow, unsigned int sequencePosition, hashEntry *entryTable, unsigned int *itemsInTable, hashEntry *e, unsigned int *hit, unsigned int *chained)
{
 		unsigned int found = 0;
		hashEntry f =*e;
		hashEntry *h= e;

			while(f.next >= 0)
			{

			h = &entryTable[f.next];
			f = entryTable[f.next];	
				if(sEQe(slidingWindow,f))	
				{
				h->repeats++;
				h->hitList[h->inHitList] = sequencePosition;
				h->inHitList++;
				assert(h->inHitList < 10); //TODO This assertion must be replaced by a check for reallocating the memory!!!!!
				
				(*hit)++;
				found = 1;
//				printEntry(*h);
				}	

			}


			//Case no matching pattern was found:
			if(found == 0)
			{
			hashEntry g;
			initEntry(&g,slidingWindow);
			g.hitList[0] = sequencePosition;
			g.inHitList++;
			assert(h->inHitList < 10); //TODO This assertion must be replaced by a check for reallocating the memory!!!!!
			
			entryTable[*itemsInTable]=g;
			h->next	 = *itemsInTable;		
			(*itemsInTable)++;
			(*chained)++;
			} 
}


//Hashing procedure. 
//-Get hash position
//-check if position is empty
//  -add to position
//-check for equality with first element
//  -add to frequency
//-check for equality with chained elements
//  -add to frequency
//-No match
//  -add to list and establish chain
void addToHash(unsigned int *slidingWindow, unsigned int sequencePosition, hashEntry **hashTable, hashEntry *entryTable, 
	       unsigned int *itemsInTable, unsigned int *hit, unsigned int *chained, unsigned int hashValue, unsigned int hashTableSize)
{
  
unsigned int pos = hashUI(slidingWindow, hashValue, hashTableSize);
	if(hashTable[pos]==NULL)
	{
		  
	  addNewToHashtable(pos, slidingWindow, sequencePosition, hashTable,  entryTable, itemsInTable);
		  
	}else{
		  
	hashEntry *e=hashTable[pos];


	if(sEQe(slidingWindow,*e))	//If we find a matching entry, simply count one more repeat
	{
	e->repeats++;
	(*hit)++;
//	printEntry(*e);

	}else{
		  
	  chainHash(slidingWindow, sequencePosition, entryTable, itemsInTable, e, hit, chained);

	}

	  
	}
	
}
//---------------------------------------------------------------------------------------------------------



//----Setup-Hashtable---------------------------------------------------------------------------------------


int populateHashTable(setting * arg, globalVariables* globalVar, char *seq, hashEntry ***hashTable, hashEntry **entryTable, unsigned int *itemsInTable)
{
  return populateHashTableWithKey(arg, globalVar, seq, hashTable, entryTable, itemsInTable, _hashValue, _hashTableSize);
}

int populateHashTableWithKey(setting * arg, globalVariables* globalVar, char *seq,  hashEntry ***hashTable, hashEntry **entryTable, unsigned int *itemsInTable, unsigned int hashValue, unsigned int hashTableSize)
{
  //Test that no mutually exclusive variables have been set
  int _assertion = 0;
  #ifdef _singleUnsigned
  _assertion++;
  #endif
  #ifdef _multiUnsigned
  _assertion++;
  #endif
  assert(_assertion==1);
  assert(basesPerByte * bitsPerBase == 8);
   #ifdef _singleUnsigned
  unsigned int _numMulti=1;
   #endif
  assert(_numMulti);
  
  unsigned int verbose = arg->verbose;


// const char seq[]="aaccGgtgttgaccGgtgtTggccGgtgtTg";
// const char seq[]="aaccGgtgttgaccGgtgtTggccGgtGgtgttgaccGgtgtTgtcGgtgttgaccGgtgtTggccGgtGgtgttgaccGTg";

/*
 const char seqT[]="agttgttagtctacgtggaccgacaaagacagattctttgagggagctaagctcaacgtagttctaacagttttttaattagagagcagatctctgatgaataaccaacgaaaaaaggcgagaaatacgcctttcaatatgctgaaacgcgagagaaaccgcgtgtcgactgtacaacagctgacaaagagattctcacttggaatgctgcagggacgaggaccattaaaactgttcatggccctggtggcgttccttcgtttcctaacaatcccaccaacagcagggatactgaagagatggggaacaattaaaaaatcaaaagccattaatgttttgagagggttcaggaaagagattggaaggatgctgaacatcttgaacaggagacgcagaactgcaggcatgatcattatgctgattccaacagtgatggcgttccatttaaccacacgtaacggagaaccacacatgatcgtcagtagacaagagaaagggaaaagtcttctgtttaaaacagaggatggtgtgaacatgtgtaccctcatggccatggaccttggtgaattgtgtgaagatacaatcacgtacaagtgtccttttctcaggcagaatgaaccagaagacatagattgttggtgcaactctacgtccacatgggtaacttatgggacgtgtaccaccacaggagaacacagaagagaaaaaagatcagtggcactcgttccacatgtgggaatgggactggagacacgaactgaaacatggatgtcatcagaaggggcctggaaacatgcccagagaattgaaacttggatcttgagacatccaggctttaccataatggcagcaatcctggcatacaccataggaacgacacatttccaaagagccctgattttcatcttactgacagctgtcgctccttcaatgacaatgcgttgcataggaatatcaaatagagactttgtagaaggggtttcaggaggaagctgggttgacatagtcttagaacatggaagctgtgtgacgacgatggcaaaaaacaaaccaacattggattttgaactgataaaaacagaagccaaacaacctgccactctaaggaagtactgtatagaggcaaagctgaccaacacaacaacagattctcgctgcccaacacaaggagaacccagcctaaatgaagagcaggacaaaaggttcgtctgcaaacactccatggtggacagaggatggggaaatggatgtggattatttggaaaaggaggcattgtgacctgtgctatgttcacatgcaaaaagaacatgaaaggaaaagtcgtgcaaccagaaaacttggaatacaccattgtgataacacctcactcaggggaagagcatgcagtcggaaatgacacaggaaaacatggcaaggaaatcaaaataacaccacagagttccatcacagaagcagagttgacaggctatggcactgtcacgatggagtgctctccgagaacgggcctcgacttcaatgagatggtgttgctgcaaatggaaaataaagcttggctggtgcacaggcaatggttcctagacctgccgttgccatggctgcccggagcggacacacaaggatcaaattggatacagaaagagacattggtcactttcaaaaatccccatgcgaagaaacaggatgttgttgttttgggatcccaagaaggggccatgcacacagcactcacaggggccacagaaatccagatgtcatcaggaaacttactgttcacaggacatctcaagtgcaggctgaggatggacaaactacagctcaaaggaatgtcatactctatgtgcacaggaaagtttaaagttgtgaaggaaatagcagaaacacaacatggaacaatagttatcagagtacaatatgaaggggacggttctccatgtaagatcccttttgagataatggatttggaaaaaagacatgttttaggtcgcctgattacagtcaacccaatcgtaacagaaaaagatagcccagtcaacatagaagcagaacctccattcggagacagctacatcatcataggagtagagccgggacaattgaagctcaactggtttaagaaaggaagttctatcggccaaatgattgagacaacaatgaggggagcgaagagaatggccattttaggtgacacagcttgggattttggatccctgggaggagtgtttacatctataggaaaggctctccaccaagttttcggagcaatctatggggctgccttcagtggggtctcatggactatgaaaatactcataggagtcattatcacatggataggaatgaattcacgcagcacctcactgtctgtgtcactagtattggtgggagtcgtgacgctgtatttgggagttatggtgcaggccgatagtggttgcgttgtgagctggaaaaacaaagaactgaagtgtggcagtgggattttcatcacagacaacgtgcacacatggacagaacaatacaagttccaaccagaatccccttcaaagctagcttcagctatccagaaagctcatgaagagggcatttgtggaatccgctcagtaacaagactggaaaatctgatgtggaaacaaataacaccagaattgaatcacattctatcagaaaatgaggtgaagttgactattatgacaggagacatcaaaggaatcatgcaggcaggaaaacgatctctgcagccccagcccactgagctgaagtattcatggaaaacatggggcaaagcgaaaatgctctctacagagtctcataaccagacctttctcattgatggccccgaaacagcagaatgccccaacacaaacagagcttggaattcgctggaagttgaagactatggctttggagtattcaccaccaatatatggctaaagttgagagaaaagcaggatgtattctgcgactcaaaactcatgtcagcggccataaaagacaacagagccgtccatgccgatatgggttattggatagaaagtgcactcaatgacacatggaagatagagaaagcctctttcatcgaagttaaaagctgccactggccaaagtcacacaccctctggagtaatggagtgttagaaagtgagatgataattccaaagaatttcgctggaccagtgtcacaacacaactacagaccaggctaccatacacaaacagcaggaccatggcatctaggtaagcttgagatggactttgatttctgcgaaggaaccacagtggtggtgactgaggactgtggaaatagaggaccctctttaagaacaactactgcctctggaaaactcataacagaatggtgctgccgatcttgcacattaccaccgctaagatacagaggtgaggacggatgctggtacgggatggaaatcagaccattgaaagagaaagaagagaatttggtcaactccttggtcacagccggacatgggcagattgacaacttttcactaggagtcttgggaatggcattgttcctggaagaaatgctcaggacccgagtaggaacgaaacatgcaatactactagttgcagtttcttttgtgacattgatcacagggaacatgtcctttagagacctgggaagagtgatggttatggtgggcgctactatgacggatgacataggtatgggcgtgacttatcttgccctactagcagccttcaaagtcagaccaa\
cttttgcagctggactactcttgagaaagttgacctccaaggaattgatgatgactaccataggaatcgtactcctctcccagagcaccataccagagaccattcttgaactgactgatgcgttagccttgggcatgatggtccttaaaatggtgagaaaaatggaaaagtatcaattggcagtgactatcatggctatcttgtgcgtcccaaatgcagtgatattacaaaacgcatggaaagtgagttgcacaatattggcagtggtgtccgtttccccactgttcttaacatcctcacagcagaaagcggattggataccattagcattgacgatcaagggtctcaatccaacagctatttttctaacaaccctttcaagaaccaacaagaaaaggagctggccactaaatgaggctatcatggcagtcgggatggtgagcattttggccagttcactcctaaagaatgacattcccatgacaggaccattagtggctggagggctcctcactgtgtgctacgtgctcactggacgatcggccgatttggaactggagagagccgccgatgtcaaatgggaagatcaggcagagatatcaggaagcagtccaatcctgtcaataacaatatcagaagatggtagcatgtcgataaaaaacgaagaggaagaacaaacactgaccatactcattagaacaggattgctggtgatctcaggactttttcctgtatcaataccaatcacggcagcagcatggtacctgtgggaagtgaagaaacaacgggctggagtattgtgggatgtcccttcacccccacccgtgggaaaggctgaactggaagatggagcctatagaatcaagcaaaaagggattcttggatattcccagatcggagccggagtttacaaagaaggaacattccatacaatgtggcatgtcacacgcggcgctgttctaatgcataaaggaaagaggattgaaccatcatgggcggacgttaagaaagacctaatatcatatggaggaggctggaagctagaaggagaatggaaggaaggagaagaagtccaggtcttggcattggagcctggaaaaaatccaagagccgtccaaacaaaacctggtcttttcaaaaccaacgccggaaccataggtgccgtatctctggacttttctcctggaacctcaggatctccaatcatcgacaaaaaaggaaaagttgtgggtctttatggtaatggtgttgttacaaggagtggagcatatgtgagtgctatagcccagactgaaaaaagtattgaagacaatccagagatcgaagatgatatttttcgaaagagaaaattgaccatcatggacctccacccaggagcgggaaagacgaagagataccttccggccatagtcagagaggctataaaacggggcctgaggacattaatcctggcccccactagagtcgtggcagctgaaatggaggaagccctaagaggacttccaataagataccaaaccccagccatcagagctgagcacaccgggcgggagattgtggacctaatgtgtcatgccacattcactatgaggctgctatcaccagttagagtgccaaattacaacctgatcatcatggacgaagcccatttcacagacccagcaagtatagcggctagaggatacatctcaactcgagtagagatgggtgaggcagctgggattttcatgacagccactcctccgggaagcagagacccattccctcagagcaatgcaccaatcatggatgaagaaagagaaatccctgaacgttcgtggagttctggacatgagtgggtcacggattttaaagggaagactgtttggttcgttccaagtataaaagcaggaaatgatatagcagcttgcctgagaaaaaatggaaagaaagtgatacaactcagtaggaagacctttgattctgagtatgtcaagactagaaccaatgattgggacttcgtggtcacaactgacatttcagaaatgggtgccaacttcaaggctgagagggttatagaccccagacgctgcatgaaaccagttatactaacagatggtgaagagcgggtgatcctggcaggacctatgccagtgacccactctagtgcagcacaaagaagagggagaataggaagaaatccaaaaaatgaaaatgaccagtacatatacatgggggaacctctggaaaatgatgaagactgtgcacactggaaagaagctaaaatgctcctagataacatcaacacacctgaaggaatcattcctagcatgttcgaaccagagcgtgaaaaggtggatgccattgatggtgaataccgcttgagaggagaagcaaggaaaacctttgtggacctaatgagaagaggagacctaccagtctggttggcctacagagtggcagctgaaggcatcaactacgcagacagaaggtggtgttttgatggaattaagaacaaccaaatcttggaagaaaatgtggaggtggaaatctggacaaaagaaggggaaaggaagaaattaaaacccagatggttggatgccaggatctactctgacccactgacgctaaaggaattcaaggagtttgcagctggaagaaagtccctgaccctgaacctaatcacagaaatgggtaggcttccaactttcatgactcagaaggcaagagacgcactggacaacttagcagtgctgcacacggctgaagcaggtggaagggcgtacaatcatgctctcagtgaactgccggagaccctggagacattgcttttactgacacttctggctacagtcacaggaggaatctttttattcttgatgagcggaaggggtatagggaagatgaccctgggaatgtgctgcataatcacggctagtattctcctatggtacgcacaaatacagccacactggatagcagcttcaataatactggagttttttctcatagttttgcttattccagaaccagaaaagcagagaacaccccaagataaccaattgacctacgttgtcatagccatcctcacagtggtggccgcaaccatggcaaacgagatgggtttcctggaaaaaacgaagaaagatctcggattgggaagcattacaacccagcaacccgagagcaacatcctggacatagatctacgtcccgcatcagcatggacgctgtatgctgtggccacaacatttgtcacaccaatgttgagacacagcattgaaaattcctcagtgaacgtgtccctaacagctattgccaaccaagccacagtgttaatgggtcttgggaaaggatggccattgtcaaagatggacatcggagttccccttctcgccattggatgctactcacaagtcaaccccataactctcacagcagctcttttcttactggtagcacattatgccatcatagggccaggactccaagcaaaagcaaccagggaagctcagaaaagagcagcagcgggcatcatgaaaaacccaactgtcgatggaataacagtgattgacctagatccaataccctatgatccaaagtttgaaaagcagttgggacaagtaatgctcctagtcctctgcgtgactcaagtgttgatgatgaggactacatgggctctgtgtgaggcgttaaccttagcgaccgggcctatctccacattgtgggaaggaaatccagggaggttttggaacac\
taccattgcagtgtcaatggctaacatttttagagggagttacttggccggagctggacttctcttttccatcatgaagaacacaaccaacacgagaaggggaactggcaacataggagagacgcttggagagaaatggaaaagccgattgaacgcattggggaaaagtgaattccagatctacaagaaaagtggaatccaggaagtggatagaaccttagcaaaagaaggcattaaaagaggagaaacggaccatcacgctgtgtcgcgaggctcagcaaaactgagatggttcgtcgagagaaatatggtcacaccagaagggaaagtagtggacctcggttgcggcagaggaggctggtcatactattgtgggggactaaagaatgtaagagaagtcaaaggcctgacaaaaggaggaccaggacatgaagaacccatccccatgtcaacatatgggtggaatctagtacgtcttcaaagtggagttgacgttttcttcactccgccagaaaagtgtgacacattgttgtgtgacataggggagtcgtcaccaaatcccacggtagaagcaggacgaacactcagagtccttaacttagtggaaaattggttgaacaacaacacccaattttgcataaaggttctcaacccatacatgccctcagtcatagaaaaaatggaagcactacaaaggaaatatggaggagccttagtgaggaatccactctcacgaaactccacacatgagatgtactgggtatccaatgcctccgggaacatagtgtcatcagtgaacatgatttcaaggatgttgatcaacagattcacaatgagacacaagaaagccacttacgagccagatgtagacctcggaagcggaacccgcaacatcggaattgaaagtgagataccaaacctagacataatcgggaaaagaatagaaaaaataaaacaagagcatgaaacatcatggcactatgaccaagaccacccatacaaaacgtgggcttaccatggcagctatgaaacaaaacaaactggatcagcatcatccatggtgaacggagtggtcagactgctgacaaaaccttgggacgtcgtccccatggtgacacagatggcaatgacagacacgactccatttggacaacagcgcgtttttaaagaaaaagtggacacgagaacccaagaaccgaaagaaggcacaaagaaactaatgaaaatcacggcagagtggctttggaaagaactagggaagaaaaagacacctaggatgtgcactagagaagaattcacaagaaaggtgagaagcaatgcagccttgggggccatattcactgatgagaacaagtggaagtcggcacgtgaggctgttgaagatagtaggttttgggagctggttgacaaggaaaggaatctccatcttgaaggaaagtgtgaaacatgtgtgtataacatgatgggaaaaagagagaagaagctaggggagttcggcaaggcaaaaggcagcagagccatatggtacatgtggcttggagcacgcttcttagagtttgaagccctaggattcttgaatgaagatcactggttctccagagagaactccttgagtggagtggaaggagaagggctgcacaagctaggttacattttaagagacgtgagcaagaaagagggaggagcaatgtatgccgatgacaccgcaggatgggacacaagaatcacactagaagacctaaaaaatgaagaaatggtaacaaaccacatggaaggagaacacaagaaactagccgaggccattttcaaattaacgtaccaaaacaaggtggtgcgtgtgcaaagaccaacaccaagaggcacagtaatggatatcatatcgagaagagaccaaagaggtagtggacaagttggtacctatggactcaatactttcaccaatatggaagcccaactaatcagacagatggagggagaaggagtcttcaaaagcattcagcacctgacagtcacagaagaaatcgccgtgcaaaactggttagcaagagtagggcgcgaaaggttatcaagaatggccatcagtggagatgattgtgttgtgaaacctttagatgacaggttcgcaagcgctttaacagctctaaatgacatgggaaaggttaggaaagacatacaacaatgggaaccttcaagaggatggaacgattggacacaagtgcccttctgttcacaccatttccatgagttaatcatgaaagacggccgcgtacttgtagttccatgcagaaaccaagatgaactgattggtagagcccgaatttcccaaggagctgggtggtctttgcgagagacggcctgtttggggaagtcctacgcccaaatgtggagcttgatgtacttccacagacgtgacctcaggctggcggctaatgctatttgctcggcagtcccatcacattgggttccaacaagtagaacaacctggtccatacacgccaaacatgaatggatgacaacggaagacatgctgacagtctggaacagggtgtggattcaagaaaacccatggatggaagacaaaactccagtggaatcatgggaggaaatcccatacttggggaaaagagaagaccaatggtgcggctcattgattgggctaacaagcagggccacctgggcaaagaacatccaaacagcaataaatcaagttagatcccttataggcaatgaggaatacacagattacatgccatccatgaaaagattcagaagagaagaggaagaggcaggagtcctgtggtagaaggcaaaactaacatgaaacaaggctagaagtcaggtcggattaagccatagtacggaaaaaactatgctacctgtgagccccgtccaaggacgttaaaagaagtcaggccattacaaatgccatagcttgagtaaactgtgcagcctgtagctccacctgagaaggtgtaaaaaatctgggaggccacaaaccatggaagctgtacgcatggcgtagtggactagcggttagaggagacccctcccttacaaatcgcagcaacaatgggggcccaaggtgagatgaagctgtagtctcactggaaggactagaggttagaggagacccccccaaaacaaaaaacagcatattgacgctgggaaagaccagagatcctgctgtctcctcagcatcattccaggcacagaacgccagaaaatggaatggtgctgttgaatcaacaggttct";
*/



int i;


unsigned int *slidingWindow;                        	//This slidingWindow will store 'sizeOfWindow' consecutive 
							//bases from the sequence.
#ifdef _singleUnsigned
  	      slidingWindow = (unsigned int *)calloc(1,sizeof(unsigned int));
	      *slidingWindow = 0;							
#endif							
#ifdef _multiUnsigned
  	      slidingWindow = (unsigned int *)calloc(_numMulti,sizeof(unsigned int));
	      for(i=0;i<_numMulti;i++)
	      slidingWindow[i] = 0;								      
#endif
unsigned int sizeOfWindow  = basesPerWindow();	 	//Each base is encoded in bitsPerBase bits, thus basesPerByte the number of
							// (8bit) bytes is the size the window.

if(verbose > 0)
printf("\nNumber of bases in the window: %u\n",sizeOfWindow );


*hashTable = realloc(*hashTable, hashTableSize * sizeof(hashEntry*));//DO NOT FORGET TO FREE THIS WHERE APPROPRIATE!!! where to put *s ?

for(i=0;i<hashTableSize;i++)
(*hashTable)[i]=NULL;

//Table holding the actual entrier.
//Allocate enough memory to be sure to store all information.
//Size will be reduced again later
*entryTable = realloc(*entryTable, globalVar->referenceSequenceLength * sizeof(hashEntry)); 
*itemsInTable=0;

unsigned int 	hit	= 0,
		chained = 0;




initWindow(slidingWindow, seq);


  if(verbose > 1)
  printf("%s",seq);
  
	for(i= sizeOfWindow-1 ; i<strlen(seq)-1 ; i++)//Go through the whole sequence. -1 because we do not want to read the newline or EOF character
	{
	
	pushToWindow(slidingWindow, seq[i]);
	    if(verbose > 1)
	    printWindow(slidingWindow);

        addToHash(slidingWindow, (i - (sizeOfWindow-1)) ,  *hashTable, *entryTable, itemsInTable, &hit, &chained, hashValue, hashTableSize);

	}



unsigned int max=0;
if(verbose > 0)
{
for(i=0;i<*itemsInTable;i++)
{
if((*entryTable)[i].repeats>max)
max=(*entryTable)[i].repeats;
}


printf("max %u\n",max);

printf("items in table: %u, hit: %u, chained %u\n", *itemsInTable,hit, chained);
}

*entryTable = realloc(*entryTable,*itemsInTable * sizeof(hashEntry)); 

free(slidingWindow);



return chained;
}


//-----------------------------------------------------------------------------------------------------------------------


//--Look-up-hashtable-entries--------------------------------------------------------------------------------------------
int getMapPostition(unsigned int *hitList, unsigned int *whichHit, unsigned int howManyPos)
{
  unsigned int max = 0;
  int whichMax = -1;
  unsigned int i;
  for(i = 0; i<howManyPos; i++)
  {
    if(hitList[whichHit[i]] > max)
    {
     max =  hitList[whichHit[i]];
     whichMax = whichHit[i];
    }
  }
  
  if(max > 1) //TODO find actual criteria! 2 is really pretty good already. A window with up to 224 bp has only a 10^-3 chance to be random if two fragments hit.
    return whichMax;
  
  return -1;
}


int checkHash(hashEntry ** hashTable, hashEntry * entryTable, unsigned int * window,  
	      unsigned int *hitTable, unsigned int *whichHit, unsigned int *howManyPos, unsigned int fragmentNumber)
{
  unsigned int i;
   unsigned int pos = hashUI(window, _hashValue, _hashTableSize);
   int sequencePosition;
    
   if(hashTable[pos] != NULL)
   {
     	hashEntry *e=hashTable[pos];


	if(sEQe(window,*e))	
	{
	  assert(e->inHitList > 0);
	  for(i = 0; i<e->inHitList; i++)
	  {
	    sequencePosition = e->hitList[i] - fragmentNumber * basesPerWindow(); 
	    if(sequencePosition < 0)//TODO actually check overlap here!
	    { 
	      continue;
	    }
	    
	    if(hitTable[sequencePosition ] == 0)
	    {
	     whichHit[*howManyPos] = sequencePosition;
	  
	    (*howManyPos)++; 	      
	    }
	    hitTable[sequencePosition ]++;

	  }
	return 1;
	}else{
		  
	 	
		hashEntry f =*e;
		

			while(f.next >= 0)
			{

			
			f = entryTable[f.next];	
				if(sEQe(window,f))	
				{
				  assert(f.inHitList > 0);
				  for(i = 0; i<f.inHitList; i++)
				  {
				    sequencePosition = f.hitList[i] - fragmentNumber * basesPerWindow(); 
				  	    if(sequencePosition < 0)
					    { 
					    continue;
					    }
					    
					    if(hitTable[sequencePosition] == 0)
					    {
					    whichHit[*howManyPos] = sequencePosition;
	  
					    (*howManyPos)++;	      
					    }
				  hitTable[sequencePosition]++;
				  
				  }
				return 1;				
				}	

			}



	}
   }
   return 0;  
}

char *trim(char *str)// From http://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
{
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while( isspace(*frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace(*(--endp)) && endp != frontp ) {}
    }

    if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    else if( frontp != str &&  endp == frontp )
            *str = '\0';

    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of endp to mean the front of the string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }


    return str;
}


int getNextNonEmptyLine(FILE * file, char ** read, unsigned int * bytes)
{
  
 unsigned int i;  

  unsigned int foundBeginning = 0;
  
  
  int seqLength =  getline(read, bytes,file);
    
	while(!foundBeginning)
	{
	i = 0;
	  while((*read)[i] == ' ' || (*read)[i] == '\t')
	  {
	  i++;
	  }
	  
	  if((*read)[i] == '\n')
	  {
	  seqLength =   getline(read, bytes,file);
	  }else{
    
	    if((*read)[i] == EOF || seqLength <= 0)
	    return 0;
    
	  foundBeginning = 1;
	  }
	}
 
    //Simply delete leading and trailing whitespaces
    trim(*read);
    
	if(*read[0] == 0)//An empty string crashes the program somewhere downstream, so simply avoid this....
	{
	*read[1] = 0;
	*read[0] = ' ';
	return 0;
	}
  
  return 1;
}

int getNextRead(FILE * file, char ** read, unsigned int * bytes, unsigned int * firstPosition)
{
 
 unsigned int i;  

  unsigned int foundBeginning = 0;
  
  int seqLength =  getline(read, bytes,file);
  
  
  unsigned int isName = 0;
  unsigned int isPlus = 0;
  
  unsigned int running = 1;
  
  while(!foundBeginning)
  {
    i = 0;
  while((*read)[i] == ' ' || (*read)[i] == '\t')
  {
    i++;
  }
  if((*read)[i] == '\n')
  {
    seqLength =   getline(read, bytes,file);
  }else{
    
  if((*read)[i] == EOF || seqLength <= 0)
    return 0;
    
    
  if((*read)[i] == '@' || (*read)[i] == '>')
  {
    isName = 1;
  }
  if((*read)[i] == '+')
  {
    isPlus = 1;
  }
  foundBeginning = 1;
  }
  }
 
  
  if(isName)
  {
   return getNextRead(file, read, bytes, &i);
  }
    if(isPlus)
  {
    getNextRead(file, read, bytes, &i);
    running = getNextRead(file, read, bytes, &i);
    
  }


  trim(*read); //TODO either rewrite with trimmed string in mind, or get firstposition to work
 if(*read[0] == 0)//An empty string crashes the program somewhere downstream, so simply avoid this....
 {
   *read[1] = 0;
   *read[0] = ' ';
   return 0;
 }
  *firstPosition = i;

  return running;
  
}

int getNextQuality(FILE * file, char ** read, unsigned int * bytes, unsigned int * firstPosition)
{
 
 unsigned int i;  

  unsigned int foundBeginning = 0;
  
  int seqLength =  getline(read, bytes,file);
  
  
//  unsigned int isName = 0;
  unsigned int isPlus = 0;
//  unsigned int isQ    = 0;
  
  unsigned int running = 1;
  
  while(!foundBeginning)
  {
    i = 0;
  while((*read)[i] == ' ' || (*read)[i] == '\t')
  {
    i++;
  }
  if((*read)[i] == '\n')
  {
    seqLength =   getline(read, bytes,file);
  }else{
    
  if((*read)[i] == EOF || seqLength <= 0)
    return 0;
    
    

  if((*read)[i] == '+')
  {
    if((*read)[i + 1] == ' ' || (*read)[i + 1] == '\n' || (*read)[i + 1] == '\t')
    isPlus = 1;

  }
  foundBeginning = 1;
  }
  }
  
    if(isPlus)
  {
   return getNextQuality(file, read, bytes, &i);
  }


  trim(*read); //TODO either rewrite with trimmed string in mind, or get firstposition to work
 if(*read[0] == 0)//An empty string crashes the program somewhere downstream, so simply avoid this....
 {
   *read[1] = 0;
   *read[0] = ' ';
 }
 
  *firstPosition = i;
  return running;
  
}

int placeFragments(hashEntry ** hashTable, hashEntry * entryTable, char * seq, unsigned int firstPosition, 
			    unsigned int * hit, unsigned int *miss, unsigned int * hitPerRound)
{
   unsigned int i, pos;
   
   unsigned int howManyPos = 0,  
		localHitPerRound= 0,
		length 		= 0,
		lhit 		= 0,
		lmiss 		= 0,
		fragments 	= 0;
		
    unsigned int *window;
  
    unsigned int num = 1;
    #ifdef _multiUnsigned
    num = _numMulti;
    #endif
  
    unsigned int * whichHit = (unsigned int *)calloc(20 * 10,sizeof(unsigned int));//TODO Again, hardcoded values that work for this data set. MUST be changed!!!!
 
    window = (unsigned int*)malloc(num * sizeof(unsigned int));
    
		
   unsigned int * hitList = (unsigned int *)calloc(17000,sizeof(unsigned int));//TODO hard coded 17000 only works for this data. Must be changed to actual value from file!!!!!!
 
   
  length = strlen(seq) - firstPosition - 1;//-1 for \n at end of string TODO check for last row whether this is true
  assert(length > 0);
  
  fragments  = floor( (double)length/(double)basesPerWindow() );
  assert(fragments > 0); //TODO if this actually happens, the read is to short and should be ignored/discarded
  
  for(i=0; i<fragments ;i++)
  {
    pos = i * basesPerWindow() + firstPosition;
    makeWindow(window, seq, pos);
    

  if(checkHash(hashTable, entryTable, window, hitList, whichHit, &howManyPos, i))
  {
      localHitPerRound++;
   lhit++;
    }else{
     lmiss++; 
    }
    
  } 
  
  *miss 	+= lmiss;
  *hit 		+= lhit;
  *hitPerRound	 = localHitPerRound;
  
 
  free(window);
  
 
  int result = getMapPostition(hitList, whichHit, howManyPos); 
  
  free(hitList);
  free(whichHit);
   
  return result;
}






//void hashMapReadsWithKey(setting arg,  hashEntry **hashTable, hashEntry *entryTable, unsigned int itemsInTable, unsigned int hashValue, unsigned int hashTableSize);
void hashMapReadsWithKey(setting arg, globalVariables globalVar, unsigned int hashValue, unsigned int hashTableSize);
//FILE* fopen64(const char *filename, const char *type);

void hashMapReads(setting arg, globalVariables globalVar)
{
   hashMapReadsWithKey( arg, globalVar, _hashValue, _hashTableSize);
}

void hashMapReadsWithKey(setting arg, globalVariables globalVar, unsigned int hashValue, unsigned int hashTableSize)
{
  
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
  
  
  unsigned int firstPosition = 0;

  unsigned int bytesToRead = 10;
  char * seq;
  seq = (char*)malloc(bytesToRead );
  

  int reversePosition = -1;
	
  unsigned int hit  = 0;
  unsigned int miss = 0;
  unsigned int mapped  = 0;
  unsigned int unmapped = 0;
  
  unsigned int hitPerRound = 0;
  unsigned int one = 0, two = 0, three = 0, four = 0;
  unsigned int numReads = 0;
  
  
	while(getNextRead(readsFile, &seq, &bytesToRead, &firstPosition))
	{
	numReads ++;

	int whichPos = placeFragments(globalVar.hashTable, globalVar.entryTable, seq, firstPosition, &hit, &miss, &hitPerRound);
		if( whichPos < 0 || whichPos > (globalVar.referenceSequenceLength - strlen(seq)) )//TODO -1 can also simply mean that the querry starts before the reference, may have to check this...
		{    
		whichPos = placeFragments(globalVar.complementHashTable, globalVar.complementEntryTable, seq, firstPosition, &hit, &miss, &hitPerRound);  
		reversePosition = globalVar.referenceSequenceLength - (whichPos + strlen(seq)) +1;
		
		//if( whichPos < 0 || reversePosition < 0 )//TODO carefully check this condition
		if( reversePosition < 0 || reversePosition > (globalVar.referenceSequenceLength - strlen(seq)) )//TODO carefully check this condition
		{    
		unmapped++;
		}else{
		mapped++; 
		//printf("numreads %u , whichPos %d, revPos %d\n", numReads, whichPos, reversePosition);
		//printf("[ReVCompl] fraction of hits within sequence%f\n", evalComplementStatisticExpensive(&(globalVar.referenceSequence[reversePosition]), seq, strlen(seq) -1));
		 
		}
		}else{
		//  printf("numreads %u , whichPos %d, revPos %d, ref length %u, seq length %u\n", numReads, whichPos, reversePosition, globalVar.referenceSequenceLength, strlen(seq));
		//  printf("[Regular] fraction of hits within sequence%f\n", evalStatisticExpensive(&(globalVar.referenceSequence[whichPos]), seq, strlen(seq) -1));
		 
		mapped++; 
		}
  
		if(hitPerRound > 3)
		four++;
		if(hitPerRound > 2)
		three++;
		if(hitPerRound > 1)
		two++;
		if(hitPerRound > 0)
		one++;
  
	}
 
  printf("fraction of mapped %f (%u / %u)\n",mapped/(double)(mapped+unmapped), mapped, (mapped+unmapped));
  printf("fraction of hits %f (%u / %u)\n",hit/(double)(hit+miss), hit, (hit+miss));
  printf("fraction of one hits %f , two %f, three %f, four %f\n",one/(double)(numReads), two/(double)(numReads), three/(double)(numReads), four/(double)(numReads));
 
  free(seq);
  fclose(readsFile);
}



























