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

void reverseSequence(char *complement, char* original);

void makeWindow(unsigned int * window, char * seq, unsigned int pos);

double evalStatisticExpensive(char * reference, char * querry, unsigned int length);

double evalComplementStatisticExpensive(char * reference, char * querry, unsigned int length);

void trimRegions(setting s, globalVariables* var, char* seq, unsigned int whichPos, unsigned int *start, unsigned int *end);

void storeSequence(globalVariables *g, resultsVector *rv, char * seq, unsigned int pos, unsigned int length);

void freeMultiList(MultiList * list, unsigned int size);

void sortReadsLeft(MultiList* list, setting s, globalVariables* g);

void sortReadsLengthIncr(MultiList * list, globalVariables * g);
void sortReadsLengthDecr(MultiList * list, globalVariables * g);

void checkIdentityWithReference(setting s, resultsVector *rv, globalVariables * g);

void buildReferenceLists(setting s, resultsVector *rv, globalVariables * g);
#endif
