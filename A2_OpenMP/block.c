#include<stdio.h>
#include<stdlib.h>

int
main()
{
  const int sz = 1000;
  double v[sz] = {0.}, a[sz][sz] = {0.};

  for(int bench = 0; bench < 100000; ++bench){
    double sum = 0.;

    for(size_t ib = 0; ib < sz; ib += bz){
      size_t ie = ib + bz < sz ? ib + bz : sz;
      for(size_t jb = 0; jb < sz; jb += bz){
        size_t je = jb + bz < sz ? jb + bz : sz;

        for(size_t ix = ib; ix < ie; ++ix)
          for(size_t jx = jb; jx < je; ++ix)
            sum += a[ix][jx] * b[jx][ix];
      }
    }
    { volatile double tmp = sum; }
  }
}
