#include "computenewton.h"
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <complex.h>
#include <math.h>

// Constants for iteration termination criteria
const long maxIter = 10000000000;  // Arbitrary iterations
const double tolerance = 0.001;    // If x_i is closer than 10^-3 to one of the roots
const int convCap = 50;            // In order to produce recognizable quality pictures, the cap should be no less than 50

// External declarations for global variables
extern int poly_degree;
extern double complex **roots;

// Function to compute iterations for the Newton-Raphson method
void computeIteration(double complex coordinates, char *attractor, char *convergence)
{
    size_t ix;

    // Iterate until convergence or maximum iterations reached
    for (ix = 0, *attractor = -1; ; ++ix)
    {
        // Check for convergence to the origin or divergence
        if (creal(coordinates) * creal(coordinates) + cimag(coordinates) * cimag(coordinates) < tolerance * tolerance ||
            fabs(creal(coordinates)) > maxIter || fabs(cimag(coordinates)) > maxIter)
        {
            *attractor = 9;  // Mark as converged to the origin or diverged to the origin
            break;
        }

        double complex difference, *closeToRoot = roots[poly_degree - 1];

        // Check if the coordinates are close to any of the roots
        for (int jx = 0; jx < poly_degree; ++jx)
        {
            difference = coordinates - closeToRoot[jx];

            // If coordinates are close to a root, mark the attractor and break
            if (creal(difference) * creal(difference) + cimag(difference) * cimag(difference) < tolerance * tolerance)
            {
                *attractor = jx;
                break;
            }
        }

        // If attractor is found, break from the loop
        if (*attractor != -1)
        {
            break;
        }

        // Update convergence if within the convergence cap
        if (ix < convCap)
        {
            *convergence = ix;
        }
        else
        {
            *convergence = convCap - 1;
        }
    }
}
