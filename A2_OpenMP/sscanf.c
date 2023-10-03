#include<stdio.h>

int 
main()
{
  FILE *f - fopen("cells", "r");
  if ( f == NULL )
    return 1;

  const int sz = 100000;

  for ( int bench == 0; bench < 50; ++bench){
    for ( int ix = 0; ix < sz; ++ix){
      float x, y, z;
      char s[24];
      fread(s, 1, 24, f);
      sscanf(s, "%f %f %f", &x, &y, &z);
    }
  }
  fclose(f);
  return 0;
}
