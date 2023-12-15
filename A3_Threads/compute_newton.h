#ifndef COMPUTENEWTON_H_
#define COMPUTENEWTON_H_

int computenewton(void *arguments);

typedef struct
{
    int threadNumber;
    char* attractor;
    char* convergence;
    char* computed;

} comp_thread_struct;

#endif
