#include "compute_newton.h"
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <complex.h>
#include <math.h>

const long maxIter = 10000000000; //Arbitrary iterations
const double tolerance = 0.001; //If x_i is closer than 10^-3 to one of the roots
const int convCap = 50; //In order to produce recognizeable quality pictures, the cap should be no less than 50
extern int poly_degree;
extern double complex **roots;

void computeIteration(double complex coordinates, char *attractor, char *convergence)
{
    size_t ix;
    for (ix = 0, *attractor = -1; ; ++ix)
    {
        if(creal(coordinates)*creal(coordinates) + cimag(coordinates)*cimag(coordinates) < tolerance*tolerance || //Closer than 0.001 to the origin
        fabs(creal(coordinates)) > maxIter || fabs(cimag(coordinates)) > maxIter ) //Whether real or imaginary part is larger than 10^10
        {
            *attractor = 9;
            break;
        }

        double complex difference, *closeToRoot = roots[poly_degree - 1];
        for(int jx = 0; jx < poly_degree; ++jx) //If x_i is closer than 0.001 to one of the roots
        {
            difference = coordinates - closeToRoot[jx];
            if(creal(difference)*creal(difference) + cimag(difference)*cimag(difference) < tolerance*tolerance)
            {
                *attractor = jx;
                break;
            }
        }
        if(*attractor != -1)
        {
            break;
        }

        switch(poly_degree)
        {
            case 1:
                coordinates -= coordinates - 1;
                break;
            case 2:
                coordinates -= (coordinates*coordinates - 1)/(2*coordinates);
                break;
            case 3:
                coordinates -= (coordinates*coordinates*coordinates - 1)/(3*coordinates*coordinates);
                break;
            case 4:
                coordinates -= (coordinates*coordinates*coordinates*coordinates - 1)/(4*coordinates*coordinates*coordinates);
                break;
            case 5:
                coordinates -= (coordinates*coordinates*coordinates*coordinates*coordinates - 1)/(5*coordinates*coordinates*coordinates*coordinates);
                break;
            case 6:
                coordinates -= (coordinates*coordinates*coordinates*coordinates*coordinates*coordinates - 1)/(6*coordinates*coordinates*coordinates*coordinates*coordinates);
                break;
            case 7:
                coordinates -= (coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates - 1)/(7*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates);
                break;
            case 8:
                coordinates -= (coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates - 1)/(8*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates);
                break;
            case 9:
                coordinates -= (coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates - 1)/(9*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates*coordinates);
                break;
            default:
                fprintf(stderr,"The degree can only range from 1 to 9 you bacon");
                exit(1);
        }

    }

    if(ix < convCap)
    {
        *convergence = ix;
    }
    else
    {
        *convergence = convCap - 1;
    }
}

int computenewton(void *arguments)
{
    const comp_thread_struct *thread_info = (comp_thread_struct*)arguments;
    int offset = thread_info -> threadNumber;
    char* attractor = thread_info -> attractor;
    char* convergence = thread_info -> convergence;
    char* computed= thread_info -> computed;

    extern mtx_t computed_mutex;
    extern int numb_rows, numb_threads;

    //Iterate through Newton-Raphson method
    for(size_t i = offset; i < numb_rows; i += numb_threads)
    {   
        double real_coordinates = -2 + (double) (4 * i) / (numb_rows - 1);
        for(size_t j = 0; j < numb_rows; ++j)
        {
            double imaginary_coordinates = -2 + (double) (4 * j) / (numb_rows - 1);
            double complex coordinates = real_coordinates + imaginary_coordinates * I;
            computeIteration(coordinates, attractor + i * numb_rows + j, convergence + i * numb_rows + j);

        }
        //Ensure that the computed row can be written
        mtx_lock(&computed_mutex);
        thread_info -> computed[i] = 1;
        mtx_unlock(&computed_mutex);
    }

      
}
