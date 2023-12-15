#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <string.h>
#include <complex.h>
#include "compute_newton.h"
#include "write_newton.h"

mtx_t computed_mutex;
int numb_rows, numb_threads, poly_degree;
double complex **roots;

static inline void parsingArguments(char *argv[]) 
{
  if (strchr(argv[1],'t') == NULL) 
  {
    char int1_string[strlen(argv[1]) - 2];
    for (int i = 2; i < strlen(argv[1]); ++i){
      int1_string[i-2] = argv[1][i];
    }
    int int1 = atoi(int1_string);

    char int2_string[strlen(argv[2]) - 2];
    for (int j = 2; j < strlen(argv[2]); ++j){
      int2_string[j-2] = argv[2][j];
    }
    int int2 = atoi(int2_string);

    numb_rows = int1;
    numb_threads = int2;

  } 
  else 
  {
    char int1_string[strlen(argv[2]) - 2];
    for (int i = 2; i < strlen(argv[2]); ++i){
      int1_string[i-2] = argv[2][i];
    }
    int int1 = atoi(int1_string);

    char int2_string[strlen(argv[1]) - 2];
    for (int i = 2; i < strlen(argv[1]); ++i){
      int2_string[i-2] = argv[1][i];
    }
    int int2 = atoi(int2_string);

    numb_rows = int1;
    numb_threads = int2;
  }
  poly_degree = atoi(argv[3]);

}

void initialize_roots() 
{
    roots = (double complex**) malloc(sizeof(double complex*) * 9);
    for (int ix = 0; ix < 9; ix++) {
        roots[ix] = (double complex*) malloc(sizeof(double complex) * (ix + 1));
    }
    // roots for d = 1
    roots[0][0] = 1 + 0 * I;
    // roots for d = 2
    roots[1][0] = 1 + 0 * I;
    roots[1][1] = -1 + 0 * I;
    // roots for d = 3
    roots[2][0] = 1 + 0 * I;
    roots[2][1] = -0.5 + 0.86603 * I;
    roots[2][2] = -0.5 - 0.86606 * I;
    // roots for d = 4
    roots[3][0] = 1 + 0 * I;
    roots[3][1] = 0 + 1 * I;
    roots[3][2] = -1 + 0 * I;
    roots[3][3] = 0 - 1 * I;
    // roots for d = 5
    roots[4][0] = 1 + 0 * I;
    roots[4][1] = 0.309017 + 0.951057 * I;
    roots[4][2] = -0.809017 + 0.587785 * I;
    roots[4][3] = -0.809017 - 0.587785 * I;
    roots[4][4] = 0.309017 - 0.951057 * I;
    // roots for d = 6
    roots[5][0] = 1 + 0 * I;
    roots[5][1] = 0.5 + 0.866025 * I;
    roots[5][2] = -0.5 + 0.866025 * I;
    roots[5][3] = -1 - 0 * I;
    roots[5][4] = -0.5 - 0.866025 * I;
    roots[5][5] = 0.5 - 0.866025 * I;
    // // roots for d = 7
    roots[6][0] = 1 + 0 * I;
    roots[6][1] = 0.62349 + 0.781831 * I;
    roots[6][2] = -0.222521 + 0.974928 * I;
    roots[6][3] = -0.900969 + 0.433884 * I;
    roots[6][4] = -0.900969 - 0.433884 * I;
    roots[6][5] = -0.222521 - 0.974928 * I;
    roots[6][6] = 0.62349 - 0.781831 * I;
    // roots for d = 8
    roots[7][0] = 1 + 0 * I;
    roots[7][1] = 0.707107 + 0.707107 * I;
    roots[7][2] = 0 + 1 * I;
    roots[7][3] = -0.707107 + 0.707107 * I;
    roots[7][4] = -1 + 0 * I;
    roots[7][5] = -0.707107 - 0.707107 * I;
    roots[7][6] = 0 - 1 * I;
    roots[7][7] = 0.707107 - 0.707107 * I;
    // roots for d = 9
    roots[8][0] = 1 + 0 * I;
    roots[8][1] = 0.766044 + 0.642788 * I;
    roots[8][2] = 0.173648 + 0.984808 * I;
    roots[8][3] = -0.5 + 0.866025 * I;
    roots[8][4] = -0.939693 + 0.34202 * I;
    roots[8][5] = -0.939693 - 0.34202 * I;
    roots[8][6] = -0.5 - 0.866025 * I;
    roots[8][7] = 0.173648 - 0.984808 * I;
    roots[8][8] = 0.766044 - 0.642788 * I;
}


int main(int argc, char *argv[])
{
    //Parsing
    parsingArguments(argv);
    printf("t=%d, l=%d, deg=%d\n",numb_threads,numb_rows,poly_degree);

    //Open files and write the headers
    FILE *convergenceFile;
    char convergenceName[26];
    sprintf(convergenceName, "newton_convergence_x%d.ppm", poly_degree);
    convergenceFile = fopen(convergenceName, "w");
    fprintf(convergenceFile, "P3\n%d %d\n255\n", numb_rows, numb_rows);

    FILE *attractorFile;
    char attractorName[25];
    sprintf(attractorName, "newton_attractors_x%d.ppm", poly_degree);
    attractorFile = fopen(attractorName, "w");
    fprintf(attractorFile, "P3\n%d %d\n255\n", numb_rows, numb_rows);

    //Initialize arrays and threads
    char *attractor = (char*)malloc(sizeof(char)*numb_rows*numb_rows);
    char *convergence = (char*)malloc(sizeof(char)*numb_rows*numb_rows);
    char *computed = (char*)calloc(numb_rows,sizeof(char)); //Used as a check

    thrd_t computational_threads[numb_threads]; //n threads for computing
    thrd_t writing_thread; //One thread for writing
    comp_thread_struct comp_thread_arg[numb_threads];
    mtx_init(&computed_mutex, mtx_plain);
    initialize_roots();
    int r;
    //Compute Newton Raphson's method
    for (int tx = 0; tx < numb_threads; ++tx)
    { 
        comp_thread_arg[tx].threadNumber = tx;
        comp_thread_arg[tx].attractor = attractor;
        comp_thread_arg[tx].convergence = convergence;
        comp_thread_arg[tx].computed = computed;

        r = thrd_create(computational_threads + tx, computenewton, (void*)(&comp_thread_arg[tx]));
        if(r != thrd_success)
        {
          fprintf(stderr,"failed to create thread\n");
          exit(1);
        }
    }

    //Write to file
    write_thread_struct write_thread_arg;
    
    write_thread_arg.convergenceFile  = convergenceFile;
    write_thread_arg.attractorFile = attractorFile;
    write_thread_arg.attractor = attractor;
    write_thread_arg.convergence = convergence;
    write_thread_arg.computed = computed;
      
    r = thrd_create(&writing_thread, writenewton, (void*)(&write_thread_arg));
    if(r != thrd_success)
    {
      fprintf(stderr,"failed to create thread\n");
      exit(1);
    }
    

    for(int tx = 0; tx < numb_threads; ++tx)
    {
      thrd_join(computational_threads[tx], NULL);
    }
    
    thrd_join(writing_thread, NULL);
    
    
    //Close/exit
    fclose(convergenceFile);
    fclose(attractorFile);

    //Free
    free(attractor);
    free(convergence);
    free(computed);
    for(size_t i = 0; i < 9; ++i)
    {
      free(roots[i]);
    }
    free(roots);
    mtx_destroy(&computed_mutex);

}
