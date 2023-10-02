#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

//initially derfine blocks
#define total_blocks 2
static int file_each;
int parrel_n = 4;
static inline int read_file(short int*,char *,int,int);
static inline void ComputeWithinBlock (short int ** block1, long int* output);
static inline void ComputeAcrossBlock (short int ** block1,short int ** block2,long int* output);

//TODO: find -t(??)
int main(int argc,char *argv[])
{
//int parrel_n = 4;
if(argc>1){
    if(argv[1][0] =='-'&&argv[1][1] == 't'){
        parrel_n = atoi(argv[1]+2);}
}


//TODO: read files and collect rows
char *filename = "cell_e5";
FILE *file = fopen(filename,"r");
fseek(file,0L,SEEK_END);
int file_length = 0;
file_length = ftell(file)/24;
//seperate whole into blocks --> set to be 2
file_each = file_length/total_blocks;
printf("%i\n",file_length);
fclose(file);

//use the collected rows --> 3465
static long int printout_list[3465];
for (int i = 0;i<3465;++i){
printout_list[i] = 0;}

//test the outlist
//for (size_t i = 0;i<5;++i){
//printf("%i\n",printout_list[i][0]);}

short int *block1_asentries = (short int*)malloc(file_each*3*sizeof(short int));
short int **block1 = (short**)malloc(sizeof(short int*)*file_each);

short int *block2_asentries = (short int*)malloc(file_each*3*sizeof(short int));
short int **block2 = (short**)malloc(sizeof(short int*)*file_each);

//converts the 1D array block1/2_asentries into a 2D array block1/2 with each row having 3 elements
for (size_t i=0;i<file_each;++i){
block1[i] = block1_asentries + 3*i;}
for (size_t i=0;i<file_each;++i){
block2[i] = block2_asentries + 3*i;}

//use OpenMP for the following for loops --> threads(4)
#pragma omp parallel for num_threads(4) schedule(dynamic)
for(size_t i = 0; i<total_blocks-1;++i){
  read_file(block1_asentries,filename,i*file_each,(i+1)*file_each);
  //for(size_t m = 0; m<10;++m){
  //printf("%i\n",block1_asentries[m]);}
  ComputeWithinBlock(block1,&printout_list[0]);
  //for (int ix = 0;ix<3465;++ix){
  //printf("%i\n",printout_list[ix]);
}
#pragma omp parallel for num_threads(4)
for(size_t j = i+1;j<total_blocks;++j){
  read_file(block2_asentries,filename,j*file_each,(j+1)*file_each);
  
  ComputeAcrossBlock(block1,block2,&printout_list[0]);
}
#pragma omp barrier
}
read_file(block1_asentries,filename,file_length-file_each,file_length);
ComputeWithinBlock(block1,&printout_list[0]);

//for (int ix = 0; ix < 10; ++ix){
//printf("%i %i %i\n",block1[ix][0],block1[ix][1],block1[ix][2]);}

//free(block1_asentries);
//free(block1);
//free(block2_asentries);
//free(block2);
for(int  ix = 0; ix < 3465;++ix){
if (printout_list[ix]!=0){
printf("%05.2f %i\n",(float)ix/100, printout_list[ix]);
}
}
}
//printout needs to be written

//print_read file



int read_file (short int *coords,char *filename, int start, int end) 
{
int size = end - start;
int lsz = 24; 
char *lines = (char*) malloc( sizeof(char)*size*lsz);
short int pos[3];

FILE *file = fopen(filename, "r");
if (file == NULL)
{
printf("error opening file\n");
return -1;
}

fseek(file, start*lsz, 0); 
fread (lines, sizeof(char), size*lsz, file); 
fclose(file);
for (int ix = 0; ix < size; ++ix)
{
int lskip = ix*lsz; 
pos[0] = (44-lines[0 + lskip]) * ((lines[ 1 + lskip]-48)*10000+ (lines[ 2 + lskip]-48)*1000
           + (lines[ 4 + lskip]-48)*100
           + (lines[ 5 + lskip]-48)*10
           + (lines[ 6 + lskip]-48)*1
);
pos[1] = (44-lines[8 + lskip]) * ((lines[9 + lskip]-48)*10000+(lines[10+lskip]-48)*1000
           + (lines[12 + lskip]-48)*100
           + (lines[13 + lskip]-48)*10
           + (lines[14 + lskip]-48)*1
);
pos[2] = (44-lines[16 + lskip]) * ((lines[17 + lskip]-48)*10000
           + (lines[18 + lskip]-48)*1000
           + (lines[20 + lskip]-48)*100
           + (lines[21 + lskip]-48)*10
           + (lines[22 + lskip]-48)*1
);
memcpy(coords+3*ix, pos, 3*sizeof(short int));
}

free(lines);
return 0;
}

/* Computes The Distance within a block */
void ComputeWithinBlock (short int ** block1,/* float * entries, short int * entries_int_comp,*/ long int * output)
{
//#pragma ... 
omp_set_num_threads(parrel_n);
//long int distentry = 0;
//{
 long int distentry;
 
#pragma omp parallel for private(distentry) reduction(+:output[:3465])
  for (int  i=0 ;i < file_each ; i++ )
//    #pragma omp for reduction(+:distentry)
 //long int distentry;

//#pragma omp simd
    for (int j=i+1; j < file_each ; j++)
  {
//printf("%i\n",block1[j][0]);

                   distentry= (long int) (sqrtf((((block1[i][0]) - (block1[j][0])  )
 * (( block1[i][0]) - (block1[j][0])) + ((block1[i][1]) - (block1[j][1]))
 * ((block1[i][1])- (block1[j][1])) + ((block1[i][2])- (block1[j][2])) *
 ( ( block1[i][2]) - (  block1[j][2]))  ))/10);
//#pragma omp critical
           output[distentry]  =  output[distentry]  + 1;

 //  printf ("%i %i\n",distentry,output[distentry] );

  }
//}
} 

/* Computes The Distance Across two blocks */
void ComputeAcrossBlock (short int ** block1,short int ** block2,/* float * entries, short int * entries_int_comp,*/ long int * output )
{
//omp_set_num_threads(5);
//long int distentry = 0;
//{
//#pragma ... reduction(+:output[:3465])

 long int distentry = 0;
 
    //short int n;
#pragma omp parallel for private(distentry) reduction(+:output[:3465])
  for (int i=0 ;i < file_each ; i++ )
 //   #pragma omp for reduction(+:distentry)

    for (int j=0; j < file_each ; j++)
  {

                     distentry= (long int) (sqrtf(((block1[i][0]) - (block2[j][0])  ) * (( block1[i][0]) - (block2[j][0])) + ((block1[i][1]) - (block2[j][1])) * ((block1[i][1])- (block2[j][1])) + ((block1[i][2])- (block2[j][2])) * ( ( block1[i][2]) - (  block2[j][2]))  )/10);
//#pragma omp critical
              output[distentry]  =  output[distentry] + 1;

// printf ("%i %i\n",distentry,output[distentry] );
}
//}

}
