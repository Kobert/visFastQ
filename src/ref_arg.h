
#ifndef _ARGS   /* Include guard */
#define _ARGS



setting initArgs(int argc, char **argv);

void freeGlobalVariables(globalVariables *globalVar);
 
void initGlobalVariables(setting * arg, globalVariables *globalVar);

void freeResults(resultsVector *rv);

void reallocResults(resultsVector *rv, unsigned int length);

void initResults(resultsVector *rv, unsigned int length);
#endif
