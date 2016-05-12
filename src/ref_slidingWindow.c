// Contains all functions that Manipulate or access the slidingWindow, or actual bases themselves.


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "referenceAssembly.h" 
#include "ref_slidingWindow.h" 


//---------MAPPING-FUNCTIONS---------------------------------------------------------------------------------------------
int mapUnsafe(char c) //returns the integer value of a DNA character
{
if(bitsPerBase == 4)
{
	switch(c)
	{
	case 'A':
	case 'a':
	return 1;

	case 'C':
	case 'c':
	return 2;

	case 'G':
	case 'g':
	return 4;

	case 'T':
	case 't':
	case 'U':
	case 'u':
	return 8;
	
	case 'N':
	case 'X':
	case '-':
	return 15;
	
	case 'W':
	case 'w':
	return 9;

	case 'S':
	case 's':
	return 6;

	case 'M':
	case 'm':
	return 3;

	case 'K':
	case 'k':
	return 12;

	case 'R':
	case 'r':
	return 5;

	case 'Y':
	case 'y':
	return 10;

	case 'B':
	case 'b':
	return 14;

	case 'd':
	case 'D':
	return 13;

	case 'H':
	case 'h':
	return 11;

	case 'V':
	case 'v':
	return 7;
	
	default:
	return -1;
	}
}
else if(bitsPerBase == 2)
{
	switch(c)
	{
	case 'A':
	case 'a':
	return 0;

	case 'C':
	case 'c':
	return 1;

	case 'G':
	case 'g':
	return 2;

	case 'T':
	case 't':
	return 3;

	default:
	printf("\nInvalid character passed to mapUnsafe function c = \"%c\". \n (Degenerate characters are not possible under this setting)\n",c);
	assert(0);
	}
}

}

unsigned int map(char c)
{
 int result = mapUnsafe(c);
    if(result >= 0)
    return (unsigned int)result;
  
  printf("\nInvalid character passed to map function c = \"%c\".\n",c);
    
    if(bitsPerBase == 2)
    printf("(Note that degenerate characters are not possible under current setting)\n");
	
  assert(0);
	
  return 0;
  
  
}
char reMap(unsigned int n) //Recover actual base from number
{

if(bitsPerBase == 4)
{
	switch(n)
	{
	case 1:
	return 'A';
	case 2:
	return 'C';
	case 4:
	return 'G';
	case 8:
	return 'T';
	case 9:
	return 'W';
	case 6:
	return 'S';
	case 3:
	return 'M';
	case 12:
	return 'K';
	case 5:
	return 'R';
	case 10:
	return 'Y';
	case 14:
	return 'B';
	case 13:
	return 'D';	
	case 11:
	return 'H';
	case 7:
	return 'V';
	case 15:
	return 'N';
	default:
	printf("invalid character passed to reMap() function. \n");
	printf("Number passed: %u\n",n);

	assert(0);
	}

}else if(bitsPerBase == 2){
switch(n)
	{
	case 0:
	return 'A';
	case 1:
	return 'C';
	case 2:
	return 'G';
	case 3:
	return 'T';
	default:
	printf("invalid character passed to reMap() function. (Degenerate characters are not implemented yet...)\n");
	printf("Number passed: %u\n",n);

	assert(0);
	}
}

}

char complementDNA(char c)
{
 	switch(c)
	{
	case 'A':
	case 'a':
	return 'T';

	case 'C':
	case 'c':
	return 'G';

	case 'G':
	case 'g':
	return 'C';

	case 'T':
	case 't':
	case 'U':
	case 'u':
	return 'A';
	
	case 'N':
	case 'X':
	case '-':
	return 'N';
	
	case 'W':
	case 'w':
	return 'W';

	case 'S':
	case 's':
	return 'S';

	case 'M':
	case 'm':
	return 'K';

	case 'K':
	case 'k':
	return 'M';

	case 'R':
	case 'r':
	return 'Y';

	case 'Y':
	case 'y':
	return 'R';

	case 'B':
	case 'b':
	return 'V';

	case 'd':
	case 'D':
	return 'H';

	case 'H':
	case 'h':
	return 'D';

	case 'V':
	case 'v':
	return 'B';
	
	default:
	return -1;
	} 
  
  
}
//-------------------------------------------------------------------------------------------------------------------------------------------------



//------------UTILITIES-FOR-SLIDINGWINDOW-----------------------------------------------------------------------------------------------------------

// Function returns the number of bases per part of the slidingwindow. 
// The slidingWindow is seperated into chunks that are easy to handle by the maschine (for example 4 * 8 bit for one unsigned int).
unsigned int basesPerPart()
{
 return (sizeof(unsigned int) * basesPerByte);
}

//Function returns the number of bases in the overall window.
unsigned int basesPerWindow()
{
#ifdef _singleUnsigned
  unsigned int _numMulti =1;
#endif
  return (basesPerPart() * _numMulti); 
}


#ifdef _singleUnsigned
char readFromWindow(unsigned int window, unsigned int position)
{
unsigned int result=window;

unsigned int i;

	for(i=0;i<position;i++)
	{
	result=(result >> bitsPerBase);
	}

	if(bitsPerBase == 4 && result==0)
	{
	printf("result is 0. This should not happen!. position is %u. window is %u\n",position, window);
	assert(result>0);
	}

if(bitsPerBase == 4)
{	
result = (result & (unsigned int)15); 	//15 is represented as 0000 0000 0000 1111
				 	//Thus, anding it with result will give only the first base in the shifted result.
}else if(bitsPerBase == 2){

result = (result & (unsigned int)3);   	//3 is represented as 0000 0000 0000 0011
					//Thus, anding it with result will give only the first base in the shifted result.
}

return reMap(result);
}
#endif
#ifdef _multiUnsigned
char readFromWindow(unsigned int window, unsigned int position)
{
unsigned int result=window;

unsigned int i;

	for(i=0;i<position;i++)
	{
	result=(result >> bitsPerBase);
	}

	if(bitsPerBase == 4 && result==0)
	{
	printf("result is 0. This should not happen!. position is %u. window is %u\n",position, window);
	assert(result>0);
	}

if(bitsPerBase == 4)
{	
result = (result & (unsigned int)15); 	//15 is represented as 0000 0000 0000 1111
				 	//Thus, anding it with result will give only the first base in the shifted result.
}else if(bitsPerBase == 2){

result = (result & (unsigned int)3);   	//3 is represented as 0000 0000 0000 0011
					//Thus, anding it with result will give only the first base in the shifted result.
}

return reMap(result);
}
#endif


#ifdef _singleUnsigned
void pushToWindow(unsigned int *before, char add)	//before should contain a list of bases. Each base takes  bitsPerBase bits space. 
							// Thus, we make room for a new base by shifting  bitsPerBase positions. Then we simply add the new base to the freed slots.
{
unsigned int after=*before;

after = (after << bitsPerBase);
if(bitsPerBase == 4)
{
assert((after & 15)==0);				
}else if(bitsPerBase == 2){
assert((after & 3)==0);
}

after = after + map(add);

*before=after;
}
#endif
#ifdef _multiUnsigned
void pushToWindow(unsigned int *before, char add)	//before should contain a list of bases. Each base takes  bitsPerBase bits space. 
							// Thus, we make room for a new base by shifting  bitsPerBase positions. Then we simply add the new base to the freed slots.
{
  unsigned int j;
  unsigned int after;
  
  unsigned int topUp;
  
  unsigned int sizeOfWindow  = basesPerPart();//sizeof(unsigned int) * basesPerByte; 
  
  for(j = _numMulti -1 ;j>0  ;j--)
  {
    after=before[j];
    topUp=before[j-1] >> bitsPerBase * (sizeOfWindow - 1);
  
    after = (after << bitsPerBase);
    
    assert((after & 15)==0);
  
    after = after + topUp;

    before[j] = after;
  }
  
  
   after=before[0];

after = (after << bitsPerBase);
if(bitsPerBase == 4)
{
assert((after & 15)==0);				
}else if(bitsPerBase == 2){
assert((after & 3)==0);
}

after = after + map(add);

before[0]=after;
}
#endif




#ifdef _singleUnsigned
void printWindow(unsigned int *slidingWindow)
{
unsigned int i;

unsigned int sizeOfWindow  = basesPerPart();// sizeof(unsigned int) * basesPerByte;

	for(i=0;i<sizeOfWindow;i++)
	{
	printf("%c",readFromWindow(*slidingWindow, i));
	}

printf("\n<------>\n");

	for(i=0;i<sizeOfWindow;i++)
	{
	printf("%c",readFromWindow(*slidingWindow,sizeOfWindow -1 - i));
	}

printf("\n\n");

}
#endif
#ifdef _multiUnsigned
void printWindow(unsigned int *slidingWindow)
{
unsigned int i,j;

unsigned int sizeOfWindow  = basesPerPart();//sizeof(unsigned int) * basesPerByte;
/*
for(j=0; j< _numMulti ; j++)
{
	for(i=0;i<sizeOfWindow;i++)
	{
	printf("%c",readFromWindow(slidingWindow[j], i));
	}
}
printf("\n<------>\n");
*/
for(j=0; j< _numMulti ; j++)
{
	for(i=0;i<sizeOfWindow;i++)
	{
	printf("%c",readFromWindow(slidingWindow[_numMulti - j - 1],sizeOfWindow -1 - i));
	}
}
//printf("\n\n");
printf("\n");
}
#endif

#ifdef _singleUnsigned
void initWindow(unsigned int *slidingWindow, const char *seq)
{
  unsigned int i;
  unsigned int sizeOfWindow  = basesPerPart();//sizeof(unsigned int) * basesPerByte;
	for(i=0;i<sizeOfWindow-1;i++)//add the first lements to the sliding window. However, leave room for one more to add.
	{
	pushToWindow(slidingWindow, seq[i]);
	}
}
#endif
#ifdef _multiUnsigned
void initWindow(unsigned int *slidingWindow, const char *seq)
{
  unsigned int i;
  unsigned int sizeOfWindow  = basesPerWindow();//sizeof(unsigned int) * basesPerByte;
	for(i=0;i< sizeOfWindow - 1;i++)//add the first lements to the sliding window. However, leave room for one more to add.
	{
	pushToWindow(slidingWindow, seq[i]);
	}
}
#endif
//----------------------------------------------------------------------------------------------------------------------------------------


void reverseComplementSequence(char *complement, char* original)
  {
   unsigned int i;
   
   unsigned int length = strlen(original);
   char* temp;
   temp = strdup(original);
   
   if(strlen(complement) < strlen(original))
   {
  printf("\nstrlen(complement) = %lu, strlen(seq) =%lu\n", (long unsigned)strlen(complement), (long unsigned)strlen(original));
  assert( strlen(complement) >= strlen(original) ); 
   }
	for(i=0; i< length; i++)
	{
	  complement[i] = complementDNA(temp[length -1  -i]);	  
	}
	
	// Add null terminater to end of string
	complement[i] = '\0';
	
	free(temp);
    
  }
  
  
void reverseSequence(char *complement, char* original)
  {
   unsigned int i;
   
   unsigned int length = strlen(original);
   
   //temp is introduced such that we have no problem if complement and original point to the same object.
   char* temp;
   temp = strdup(original);
   
   if(strlen(complement) < strlen(original))
   {
  printf("\nstrlen(complement) = %lu, strlen(seq) =%lu\n", (long unsigned)strlen(complement), (long unsigned)strlen(original));
  assert( strlen(complement) >= strlen(original) ); 
   }
	for(i=0; i< length; i++)
	{
	  complement[i] = temp[length -1  -i];	  
	}
	
	// Add null terminater to end of string
	complement[i] = '\0';
	
	free(temp);
    
  }  
  
//-------USED-FOR-Hash-Look-up--------------------------------------------------------



//TODO can be faster by pushing only to needed window fragment, not alwasys from right!
// However, dedicated 8 base (unsigned int) version will make this redundant
void makeWindow(unsigned int * window, char * seq, unsigned int pos)
{
unsigned int i;
    unsigned int sizeOfWindow  = basesPerWindow();//sizeof(unsigned int) * basesPerByte;
    for(i=0; i< sizeOfWindow; i++)//add the first lements to the sliding window. However, leave room for one more to add.
	{
	assert(pos+i < strlen(seq));
	pushToWindow(window, seq[pos+i]);
	}
//	  printWindow(window);
}
//--------------------------------------------------------------------------------------

//------Statistical-Tests---------------------------------------------------------------

double evalStatisticExpensive(char * reference, char * querry, unsigned int length)
{

 unsigned int i;
 unsigned int hit  = 0;
	      
	for(i = 0; i<length; i++)
	{
	  //Mapping and remapping is done to avoid inequality due to different representations
	  if(reMap(map(*reference)) == reMap(map(querry[i])))
	  {
	    hit++;
	  }
	  
	  reference++;
	}
 
 return (double)hit/length;
}

double evalComplementStatisticExpensive(char * reference, char * querry, unsigned int length)
{

 unsigned int i;
 unsigned int hit  = 0;
	      
	//for(i = length -1; i>=0 ; i--)
	for(i = 0; i<length ; i++)
	{

	  if(reMap(map(*reference)) == complementDNA(querry[length - i -1]))
	    hit++;
	
	  reference++;
	  
	}
 
 return (double)hit/length;
}


//TODO if this function is worth it, there is lots of room for improvement. Quality wise and runtime wise.... 
/*
 void trimRegions(setting s, globalVariables *var, char* seq, unsigned int whichPos, unsigned int *start, unsigned int *end)
{

 unsigned int i;
 
 unsigned int hit = 12;
 unsigned int outOf = 16;

 unsigned int count =0,
	      lstart = 0,
	      lend = strlen(seq),
	      sum = 0; 
  
 //unsigned int last[16];
	       unsigned int *last;
	       last = (unsigned int *)malloc(outOf*sizeof(unsigned int));
 
//Forward unaligned region
for(i = 0; i<outOf; i++)
 {
   if(reMap(map(seq[i]))==reMap(map(var->referenceSequence[whichPos + i])))
   {
   last[i]=1;
   sum++;     
   }else{
   last[i]=0;     
   }
 }

 
 while(sum < hit && i < strlen(seq))
 {
   sum = sum - last[count];
   
  if(reMap(map(seq[i]))==reMap(map(var->referenceSequence[whichPos + i])))
   {
   last[count]=1;
   sum++;     
   }else{
   last[count]=0;     
   }
  
   count++;
   count = count % outOf;
   
   i++;
 }
 
lstart = i - outOf;
 



//Tailend unaligned region
sum =0;

for(i = 0; i<outOf; i++)
 {
   if(reMap(map(seq[strlen(seq) - i -1]))==reMap(map(var->referenceSequence[whichPos + strlen(seq) - i -1])))
   {
   last[i]=1;
   sum++;     
   }else{
   last[i]=0;     
   }
 }

 
 while(sum < hit && i < strlen(seq))
 {
   sum = sum - last[count];
   
   if(reMap(map(seq[strlen(seq) - i -1]))==reMap(map(var->referenceSequence[whichPos + strlen(seq) - i -1])))
   {
   last[count]=1;
   sum++;     
   }else{
   last[count]=0;     
   }
  
   count++;
   count = count % outOf;
   
   i++;
 }
lend = strlen(seq) - i + outOf;
 

*start = lstart;
*end   = lend;

 var->trimmed += lstart + (strlen(seq) - lend);
 var->kept += lend - lstart;
 
 //printf("%u %.20f, (%f + %f/%u)/%u\n",var->avgTrimmedOf,var->avgRatioTrimmed, var->avgRatioTrimmed * var->avgTrimmedOf, (double)(lstart + (strlen(seq) - lend)),(strlen(seq)), (var->avgTrimmedOf + 1));
 var->avgRatioTrimmed = (double)(var->avgRatioTrimmed * var->avgTrimmedOf + (lstart + (strlen(seq) - lend))/(double)(strlen(seq))  )/(var->avgTrimmedOf + 1);
 var->avgTrimmedOf += 1;
 
 //printf("%llu %llu %.30f\n",var->trimmed, var->kept, var->avgRatioTrimmed);
 
if(0 &&( *start > 0 || *end<strlen(seq)))
{
      printf("\n");
      
    for(i=0;i<*start;i++)
  printf("%c",seq[i]);
  
  printf(" || ");
  
  for(i=*start;i<*end;i++)
  printf("%c",seq[i]);
  
  printf(" || ");
  
  for(i=*end;i<strlen(seq);i++)
  printf("%c",seq[i]);
  
  printf("\n");
  
      for(i=0;i<*start;i++)
	  printf("%c",reMap(map(var->referenceSequence[whichPos + i])));
      
        printf(" || ");
      
  for(i=*start;i<*end;i++)
  printf("%c",reMap(map(var->referenceSequence[whichPos + i])));
  
    printf(" || ");
    
      for(i=*end;i<strlen(seq);i++)
	  printf("%c",reMap(map(var->referenceSequence[whichPos + i])));
      
    printf("\n");
    printf("Which pos = %u, strlen = %u length of ref = %u.\n",whichPos, strlen(seq), strlen(var->referenceSequence));
printf("0 - %u - %u - %u\n", *start, *end,  strlen(seq));
}
 free(last);
}

*/
void trimRegions(setting s, globalVariables *var, char* seq, unsigned int whichPos, unsigned int *start, unsigned int *end)
{

 unsigned int i;
 
 // TODO these numbers are somewhat arbitrary and only chosen to give results similar to bowties output...
 //unsigned int hit = 14;
 //unsigned int outOf = 16;
unsigned int hit = 8;
unsigned int outOf = 8;

 unsigned int count =0,
	      lstart = 0,
	      lend = strlen(seq),
	      sum = 0; 
char c;
  
 //unsigned int last[16];
	       unsigned int *last;
	       last = (unsigned int *)malloc(outOf*sizeof(unsigned int));
 
//Forward unaligned region
for(i = 0; i<outOf; i++)
 {
   c = reMap(map(seq[i]));
   if(c==reMap(map(var->referenceSequence[whichPos + i])))
   {
   last[i]=1;
   sum++;     
   }else{
   last[i]=0;     
   }
 }

 
 while(sum < hit && i < strlen(seq))
 {
   sum = sum - last[count];
   
   c = reMap(map(seq[i])); 
  if(c==reMap(map(var->referenceSequence[whichPos + i])))
   {
   last[count]=1;
   sum++;     
   }else{
   last[count]=0;     
   }
  
   count++;
   count = count % outOf;
   
   i++;
 }
 
lstart = i - outOf;
 



//Tailend unaligned region
sum =0;

for(i = 0; i<outOf; i++)
 {
   c = reMap(map(seq[strlen(seq) - i -1]));
   
   if(c==reMap(map(var->referenceSequence[whichPos + strlen(seq) - i -1])))
   {
   last[i]=1;
   sum++;     
   }else{
   last[i]=0;     
   }
 }

 
 while(sum < hit && i < strlen(seq))
 {
   sum = sum - last[count];
   c = reMap(map(seq[strlen(seq) - i -1]));
   if(c==reMap(map(var->referenceSequence[whichPos + strlen(seq) - i -1])))
   {
   last[count]=1;
   sum++;     
   }else{
   last[count]=0;     
   }
  
   count++;
   count = count % outOf;
   
   i++;
 }
lend = strlen(seq) - i + outOf;
 
// TODO remove those two lines
lstart += 0;
lend -= 0;

*start = lstart;
*end   = lend;

 var->trimmed += lstart + (strlen(seq) - lend);
 var->kept += lend - lstart;
 
 //printf("%u %.20f, (%f + %f/%u)/%u\n",var->avgTrimmedOf,var->avgRatioTrimmed, var->avgRatioTrimmed * var->avgTrimmedOf, (double)(lstart + (strlen(seq) - lend)),(strlen(seq)), (var->avgTrimmedOf + 1));
 var->avgRatioTrimmed = (double)(var->avgRatioTrimmed * var->avgTrimmedOf + (lstart + (strlen(seq) - lend))/(double)(strlen(seq))  )/(var->avgTrimmedOf + 1);
 var->avgTrimmedOf += 1;
 
 //printf("%llu %llu %.30f\n",var->trimmed, var->kept, var->avgRatioTrimmed);
 
if(0 &&( *start > 0 || *end<strlen(seq)))
{
      printf("\n");
      
    for(i=0;i<*start;i++)
  printf("%c",seq[i]);
  
  printf(" || ");
  
  for(i=*start;i<*end;i++)
  printf("%c",seq[i]);
  
  printf(" || ");
  
  for(i=*end;i<strlen(seq);i++)
  printf("%c",seq[i]);
  
  printf("\n");
  
      for(i=0;i<*start;i++)
	  printf("%c",reMap(map(var->referenceSequence[whichPos + i])));
      
        printf(" || ");
      
  for(i=*start;i<*end;i++)
  printf("%c",reMap(map(var->referenceSequence[whichPos + i])));
  
    printf(" || ");
    
      for(i=*end;i<strlen(seq);i++)
	  printf("%c",reMap(map(var->referenceSequence[whichPos + i])));
      
    printf("\n");
    printf("Which pos = %u, strlen = %lu length of ref = %lu.\n",whichPos, (long unsigned)strlen(seq), (long unsigned)strlen(var->referenceSequence));
printf("0 - %u - %u - %lu\n", *start, *end,  (long unsigned)strlen(seq));
}
 free(last);
}



//----------------------------------------------------------------------------------------
//TODO can be refined alot. For example by hashing stuff for the reference in advance
int matchesReference(char* reference, char* seq, unsigned int length)
{
  unsigned int i;
  for(i = 0; i < length ; i++)
  {
    if(map(seq[i]) != map(reference[i]))
      return 0;
  }
  
  return 1;
}



void storeSequence(globalVariables *g, resultsVector* rv, char * seq, unsigned int pos, unsigned int length)
{
  

  
  if(g->numReads > g->allocatedNumReads -1)
  {
    g->allocatedNumReads = g->allocatedNumReads * 2;
    
    time_t timer;
    char buffer[10];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

 //   strftime(buffer, 26, "%Y:%m:%d %H:%M:%S", tm_info);
    strftime(buffer, 10, "%H:%M:%S", tm_info);
    printf("[%s] %u Reads processed for mapping...\n", buffer, g->allocatedNumReads/2);
    fflush(stdout);
    
    g->read = (Read*) realloc(g->read, g->allocatedNumReads * sizeof(Read));
    
  }

 Read *r = &(g->read[g->numReads]);
 r->pos = pos;
 r->length = length;
 
  if(matchesReference(&(g->referenceSequence[pos]), seq, length))
  {
  r->matchesReference = 1;  
  rv->numReferenceMatches++;
  }else{
  r->seq = (char*) malloc( (length + 1) * sizeof(char) );
  memcpy( r->seq, seq, length );
  r->seq[length] = '\0';
  }
  
  g->numReads++;
}



void freeMultiList(MultiList * list, unsigned int size)
{
  unsigned int i;
 // printf("bef loop\n");
  for(i = 0; i < size ; i++)
  {
    MultiList * l = &(list[i]);
    if(l->allocatedNum > 0)
    {
      free(l->e);
    }
  }
 // printf("bef last free\n");
  free(list);
 // printf("af last free\n");
}

void addToBucket(MultiList * buckets, unsigned int pos, unsigned int id)
{
  MultiList * l;
  l = &buckets[pos];
  if( l->allocatedNum == 0 )
  {
    l->allocatedNum = 128;
    l->e = (unsigned int*) malloc( ( l->allocatedNum ) * sizeof(unsigned int) );
  }else if(l->allocatedNum < l->num + 1 ){
   l->allocatedNum = 2*l->allocatedNum;
    l->e = (unsigned int*)realloc( l->e , ( l->allocatedNum ) * sizeof(unsigned int)  );
  }
  
  l->e[l->num] = id;
  
  l->num++;
  
}


void bucketsToList(MultiList * buckets, unsigned int numberOfBuckets, unsigned int * list)
{
  unsigned int i,j;
  unsigned int count = 0;
  for(i = 0; i < numberOfBuckets ; i++)
  {
    MultiList * l = &buckets[i];
    
    for(j = 0; j < l->num ; j++)
    {
      list[count] = l->e[j];
      count++;
    }
  }
  
  
}




void sortReadsLeft(MultiList * list, setting s, globalVariables* g)
{
  unsigned int i;
  unsigned int pos;
  unsigned int max;
 // printf("bef forloop in sort\n");
  for(i = 0; i < g->numReads ; i++)
  {
 //pos = (g->read)->pos;
  pos = (g->read[i]).pos;  
  assert( pos < g->referenceSequenceLength );//If this is violated, make sure it happens only on pos = g->... . If it does, check the place where we restrict pos in the whichposfct.
  
//  max = strlen((g->read[i]).seq);
    max = (g->read[i]).length;
    assert(max > 0);
  
  MultiList * l = &list[pos];
//  printf("bef if\n");
  if( l->allocatedNum == 0 )
  {
 //   printf(" in if\n");
    l->allocatedNum = 128;
    l->e = (unsigned int*) malloc( ( l->allocatedNum ) * sizeof(unsigned int) );
  }else if(l->allocatedNum < l->num + 1 ){
//    printf("in else: l->allocatedNum %u, l->num %u \n", l->allocatedNum, l->num);
    l->allocatedNum = 2*l->allocatedNum;
    l->e = (unsigned int*)realloc( l->e , ( l->allocatedNum ) * sizeof(unsigned int)  );
  }
 // printf("af if \n"); 
  l->e[l->num] = i;
  l->num++;
  
  if(max > l->maxLength)
  {
    l->maxLength = max;
  }
  
  }
//  printf("af for in sort\n");
}




void sortReadsLengthIncr(MultiList * list, globalVariables * g)
{
    unsigned int i,j, length;
    for(i = 0; i < g->referenceSequenceLength ; i++)
  {

    //List that we will read from
    MultiList dup = list[i];
    
    //Buckets for bucketsorting the entries
    MultiList * buckets;
    buckets  = (MultiList*)calloc( dup.maxLength + 1 , sizeof(MultiList) );

    for(j = 0; j <  dup.num; j++)
    {
//     length = strlen( (g->read[dup.e[j]]).seq );
     length = (g->read[dup.e[j]]).length;
     if(!(g->read[dup.e[j]]).matchesReference)
     {
       assert((length == strlen( (g->read[dup.e[j]]).seq ) ) && "Stored length does not match measured length!");
     }
    addToBucket(buckets, length, dup.e[j]);   
    }
    
    
    //List that we will manipulate
    MultiList * l = &list[i];
    bucketsToList(buckets, dup.maxLength + 1, l->e);
    
    freeMultiList(buckets, dup.maxLength + 1);
  }
  
  
  
}

void sortReadsLengthDecr(MultiList * list, globalVariables * g)
{
    unsigned int i,j, length;
    for(i = 0; i < g->referenceSequenceLength ; i++)
  {

    //List that we will read from
    MultiList dup = list[i];
    
    //Buckets for bucketsorting the entries
    MultiList * buckets;
    buckets  = (MultiList*)calloc( dup.maxLength + 1 , sizeof(MultiList) );

    for(j = 0; j <  dup.num; j++)
    {
//     length = strlen( (g->read[dup.e[j]]).seq );
     length = (g->read[dup.e[j]]).length;
     
     if(!(g->read[dup.e[j]]).matchesReference)
     {
       assert((length == strlen( (g->read[dup.e[j]]).seq ) ) && "Stored length does not match measured length!");
     }
     
     addToBucket(buckets, dup.maxLength - length, dup.e[j]);   
    }
    
    
    //List that we will manipulate
    MultiList * l = &list[i];
    bucketsToList(buckets, dup.maxLength + 1, l->e);
    
    freeMultiList(buckets, dup.maxLength + 1);
  }
  
  
  
}


void checkIdentityWithReference(setting s, resultsVector *rv, globalVariables * g)
{
  
  
    unsigned int i;
    unsigned int pos;
 
  for(i = 0; i < g->numReads ; i++)
  {
  pos = (g->read[i]).pos;
  char * seq = strdup((g->read[i]).seq);
  
  if( matchesReference(&(g->referenceSequence[pos]), seq, strlen(seq)) )
  {
    Read * r = &(g->read[i]);
    
    //Make sure this has not been set and freed before
    assert(!r->matchesReference && "Variable assigned already!");
    r->matchesReference = 1;
    //   (g->read[i]).matchesReference = 1;
   free(r->seq);
   
   //free((g->read[i]).seq);
   
   rv->numReferenceMatches++;
  }else{
 //  printf("\n");
   //printf("%s\n", seq);
   //printf("%.*s", strlen(seq), &g->referenceSequence[pos]);
   //printf("\n");
   //printf("\n"); 
   //fflush(stdout);
  }
  free(seq);
  }
  
  
}


void buildReferenceLists(setting s, resultsVector *rv, globalVariables * g)
{
  
  
}



