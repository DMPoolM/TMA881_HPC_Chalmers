#include<stdio.h>
#include<stdlib.h>

float 
parse_coord(
  char *s)
{
  float x;
  x = (s[1] - '0') * 10.f +
      (s[2] - '0') +
      (s[4] - '0') * 0.1f +
      (s[5] - '0') * 0.01f +
      (s[6] - '0') * 0.001f;
  if (s[0] == '-'){
        return -x;
    else{
      return x;
    }
  }
  
}


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
      if (s[0] == '-'){
        x = -x;
      }
      fread(s, 1, 24, f);
      atof(s + 0);
      atof(s + 8);
      atof(s + 16);
    }
  }
  fclose(f);
  return 0;
}
