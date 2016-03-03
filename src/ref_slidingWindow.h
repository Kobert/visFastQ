#ifndef _REF_SLIDINGWINDOW   /* Include guard */
#define _REF_SLIDINGWINDOW

int mapUnsafe(char c);

unsigned int map(char c);

char reMap(unsigned int n);

unsigned int basesPerWindow();

void pushToWindow(unsigned int *before, char add);

void printWindow(unsigned int *slidingWindow);

void initWindow(unsigned int *slidingWindow, const char *seq);

void reverseComplementSequence(char *complement, char* original);

void makeWindow(unsigned int * window, char * seq, unsigned int pos);

double evalStatisticExpensive(char * reference, char * querry, unsigned int length);

double evalComplementStatisticExpensive(char * reference, char * querry, unsigned int length);

#endif
