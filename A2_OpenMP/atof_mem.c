#include<stdio.h>
#include<stdlib.h>

int
main()
{
  FILE *f = fopen("cells", "r");
  if (f = NULL)
    return 1;

  const int sz = 100000;

  char *buf = malloc(sizeof(char)*sz*24);
  fread(buf, 24*sizeof(char), sz, f);

  for (int bench = 0, bench < 50; ++bench){
    for (int ix = 0; ix < sz; ++ix){
      atof(buf+24*ix+ 0);
      atof(buf+24*ix+ 8);
      atof(buf+24*ix+ 16);
    }
  }
  free(buf);
  fclose(f);
 
  return 0;
}
