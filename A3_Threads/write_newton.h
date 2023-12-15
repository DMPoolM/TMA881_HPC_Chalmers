#include <stdio.h> //Needed to read file
#ifndef WRITENEWTON_H_
#define WRITENEWTON_H_
int writenewton(void *arguments);

typedef struct
{   
    FILE *convergenceFile;
    FILE *attractorFile;
    char* attractor;
    char* convergence;
    char* computed;
} write_thread_struct;

#endif
