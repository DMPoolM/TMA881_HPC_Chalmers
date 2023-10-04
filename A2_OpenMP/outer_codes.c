#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <math.h>

//TODO: transfer the matrix of n rows, 3 columns to 3 rows, n columns
static short colSize = 3;
static size_t count[3465]; //Maximal number of distances

static inline void distancesWithinBlock(size_t currentBlockLoad, float** block)
{   
    extern size_t count[];
    float currentCoordinates[colSize], innerProduct;
    int totalDistance;
    size_t i,j;
    #pragma omp parallel for private(i,j,innerProduct, totalDistance,currentCoordinates) shared(block,currentBlockLoad) reduction(+:count)
    for(i = 0; i < currentBlockLoad - 1; ++i) //currentBlockLoad = number of rows in the block
    {
        currentCoordinates[0] = block[i][0];
        currentCoordinates[1] = block[i][1];
        currentCoordinates[2] = block[i][2];
        for(j = i + 1; j < currentBlockLoad; ++j)
        {
          //计算相邻两列之间的距离 -> innerProduct = (x0-x1)^2 + (y0-y1)^2 + (z0-z1)^2
            innerProduct = (currentCoordinates[0] - block[j][0])*(currentCoordinates[0] - block[j][0]) +
            (currentCoordinates[1] - block[j][1]) * (currentCoordinates[1] - block[j][1]) +
            (currentCoordinates[2] - block[j][2]) * (currentCoordinates[2] - block[j][2]);
            totalDistance = (int)(100*sqrtf(innerProduct));
            
            count[totalDistance] += 1;
            
        }
    }
}
static inline void distancesBetweenBlocks(size_t currentBlockLoad, float** block,float previousBlockValues[colSize])
{
    extern size_t count[];
    float innerProduct;
    int totalDistance;
    size_t i;
    #pragma omp parallel for private(i,innerProduct,totalDistance) shared(block,currentBlockLoad, previousBlockValues) reduction(+:count)
    for(i = 0; i < currentBlockLoad; ++i)
    {
        innerProduct = (previousBlockValues[0] - block[i][0]) * (previousBlockValues[0] - block[i][0]) +
        (previousBlockValues[1] - block[i][1]) * (previousBlockValues[1] - block[i][1]) +
        (previousBlockValues[2] - block[i][2]) * (previousBlockValues[2] - block[i][2]);

        totalDistance = (int)(sqrtf(innerProduct)*100);
        count[totalDistance] += 1;
    }
}

//Maximal theoretical rows = (5*1024*1024)/(bytes of a certain datatype)
//从命令行参数中获取一个整数作为线程数，并设置OpenMP的线程数
int main(int argc, char *argv[])
{
    char int_string[strlen(argv[1]) - 2];
    for (int i = 2; i < strlen(argv[1]); ++i){
      int_string[i-2] = argv[1][i];
    }
    int numThreads = atoi(int_string);
    
    omp_set_num_threads(numThreads);
    FILE *file;
    file = fopen("cells","r");
    fseek(file, 0L, SEEK_END);
    long int res = ftell(file); 
    size_t rowCount = res/24; // <-- ftell 用于确定文件大小，随后算出文件有多少行
    size_t maximumRowsLoad = 100000; 
    size_t numOfBlocksNeeded; 

    if(rowCount % maximumRowsLoad) //Number of blocks needed. Tells whether the last contains fewer rows than the rest, and if it is it adds an additional block
    {
        numOfBlocksNeeded = rowCount/maximumRowsLoad + 1;
    }
    else
    {
        numOfBlocksNeeded = rowCount/maximumRowsLoad;
    }
