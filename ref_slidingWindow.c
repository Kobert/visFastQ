// Contains all functions that Manipulate or access the slidingWindow, or actual bases themselves.


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

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
	printf("Invalid character passed to map function c = %c. \n (Degenerate characters are not possible under this setting)\n",c);
	assert(0);
	}
}

}

unsigned int map(char c)
{
 int result = mapUnsafe(c);
    if(result >= 0)
    return (unsigned int)result;
  
  printf("Invalid character passed to map function c = %c.\n",c);
    
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
   
	for(i=0; i< strlen(original); i++)
	{
	  complement[i] = complementDNA(original [strlen(original) -1  -i]);	  
	}
    
  }
  
  
//-------USED-FOR-Hash-Look-up--------------------------------------------------------



//TODO can be faster by pushing only to needed window fragment, not alwasys from right!
// However, dedicated 8 base (unsigned int) version makes this redundant
void makeWindow(unsigned int * window, char * seq, unsigned int pos)
{
unsigned int i;
    unsigned int sizeOfWindow  = basesPerWindow();//sizeof(unsigned int) * basesPerByte;
    for(i=0; i< sizeOfWindow; i++)//add the first lements to the sliding window. However, leave room for one more to add.
	{
	assert(pos+i < strlen(seq));
	pushToWindow(window, seq[pos+i]);
	}
//  printWindow(window);
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