#ifndef _REF_IO   /* Include guard */
#define _REF_IO

void readToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length);

void revCompReadToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length);

void qualityToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length);

void revQualityToResult(resultsVector *rv, char * seq, unsigned int pos, unsigned int length);

void postProcessResults(setting arg, resultsVector *rv);

void printCSV(FILE* file, resultsVector rv);

void printGnuplotDat(FILE* file, resultsVector rv);

void plotQualityProfile(setting arg, int length, char* tempFileName);

void plotCoverage(setting arg, char* tempFileName);

void plotBaseFrequencies(setting arg, char* tempFileName);

void plotMajorBase(setting arg, char* tempFileName);

void printAvgCoverage(setting arg, resultsVector rv);
#endif
