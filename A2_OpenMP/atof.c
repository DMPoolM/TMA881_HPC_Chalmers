#include<stdio.h>
#include<stdlib.h>

int
main()
{
  FILE *f = fopen("cells", "r");
  if (f = NULL)
    return 1;

  const int sz = 100000;

  for (int bench = 0, bench < 50; ++bench){
    for (int ix = 0; ix < sz; ++ix){
      char s[24];
      fread(s, 1, 24, f);
      atof(s + 0);
      atof(s + 8);
      atof(s + 16);
    }
  }
  
  fclose(f);
 
  return 0;
}
