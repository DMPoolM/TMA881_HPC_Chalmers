#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <math.h>

//TODO: transfer the matrix of n rows, 3 columns to 3 rows, n columns
static short ColSize = 3;
static size_t count[3465]; //Maximal number of distances --> sqrt((10-(-10))^2 *3)

// Determain two functions that used to calculate the distances, distancesWithinBlock and distancesBetweenBlocks
static inline void distancesWithinBlock(size_t CurrentBlockLoad, float** block)
{   
    extern size_t count[];
    float CurrentCoordinates[ColSize], InnerProduct;
    int TotalDistance;
    size_t i,j;
    #pragma omp parallel for private(i,j,InnerProduct, TotalDistance,CurrentCoordinates) shared(block,CurrentBlockLoad) reduction(+:count)
    for(i = 0; i < CurrentBlockLoad - 1; ++i) //CurrentBlockLoad = number of rows in the block
    {
        CurrentCoordinates[0] = block[i][0];
        CurrentCoordinates[1] = block[i][1];
        CurrentCoordinates[2] = block[i][2];
        for(j = i + 1; j < CurrentBlockLoad; ++j)
        {
          
            //计算相邻两列之间的距离 Calculate the distance between columns within one block-> InnerProduct = (x0-x1)^2 + (y0-y1)^2 + (z0-z1)^2
            InnerProduct = (CurrentCoordinates[0] - block[j][0])*(CurrentCoordinates[0] - block[j][0]) +
            (CurrentCoordinates[1] - block[j][1]) * (CurrentCoordinates[1] - block[j][1]) +
            (CurrentCoordinates[2] - block[j][2]) * (CurrentCoordinates[2] - block[j][2]);
            TotalDistance = (int)(100*sqrtf(InnerProduct));
            
            count[TotalDistance] += 1;
            
        }
    }
}
static inline void distancesBetweenBlocks(size_t CurrentBlockLoad, float** block,float PreviousBlockValues[ColSize])
{
    extern size_t count[];
    float InnerProduct;
    int TotalDistance;
    size_t i;
    #pragma omp parallel for private(i,InnerProduct,TotalDistance) shared(block,CurrentBlockLoad, PreviousBlockValues) reduction(+:count)
    for(i = 0; i < CurrentBlockLoad; ++i)
    {
        //计算本Block与之前的Blocks之间的距离 Calculate the distances between columns in this block and those in previous blocks
        InnerProduct = (PreviousBlockValues[0] - block[i][0]) * (PreviousBlockValues[0] - block[i][0]) +
        (PreviousBlockValues[1] - block[i][1]) * (PreviousBlockValues[1] - block[i][1]) +
        (PreviousBlockValues[2] - block[i][2]) * (PreviousBlockValues[2] - block[i][2]);

        TotalDistance = (int)(sqrtf(InnerProduct)*100);
        count[TotalDistance] += 1;
    }
}

//Maximal theoretical rows = (5*1024*1024)/(bytes of a certain datatype)
//从命令行参数中获取一个整数作为线程数，并设置OpenMP的线程数 Grab the number in command ./xxxxxx -tx
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
    size_t MaxRowsLoad = 100000; // <-- set to be 100000
    size_t numOfBlocksNeeded; 

    // Set choices for the devision methods
    if(rowCount % MaxRowsLoad) //Number of blocks needed. Tells whether the last contains fewer rows than the rest, and if it is it adds an additional block
    {
        numOfBlocksNeeded = rowCount/MaxRowsLoad + 1;
    }else {
        numOfBlocksNeeded = rowCount/MaxRowsLoad;
    }

    // Determines number of rows each block receives
    size_t MaxRowsInBlock = rowCount;
    size_t numOfRowsInBlock[numOfBlocksNeeded], firstBlock[numOfBlocksNeeded]; 
    for(size_t i = 0; i < numOfBlocksNeeded; ++i)
    {
        if(MaxRowsInBlock >= MaxRowsLoad)
        {
            numOfRowsInBlock[i] = MaxRowsLoad;
            MaxRowsInBlock -= MaxRowsLoad;
        }else {
            numOfRowsInBlock[i] = MaxRowsInBlock;
        }
        firstBlock[i] = i * MaxRowsLoad;
    }

    //Start of calculating all blocks
    for(size_t i = 0; i < numOfBlocksNeeded; ++i) // Set parameters through for loop, prepare for run the functions
    {
        size_t CurrentBlockLoad = numOfRowsInBlock[i]; 
        float *blockEntries = (float*)malloc(sizeof(float) * CurrentBlockLoad * ColSize);
        float **block = (float**)malloc(sizeof(float*) * CurrentBlockLoad);
        
        for(size_t j = 0; j < CurrentBlockLoad; j++)
        {
            block[j] = blockEntries + j * ColSize;
        }

        fseek(file, firstBlock[i] * 24L, SEEK_SET);
        size_t ix;
        char stringLine[25]; 
        #pragma omp parallel for private(ix, stringLine) shared(block,CurrentBlockLoad)
        for(ix = 0; ix < CurrentBlockLoad; ++ix)
        {
            fgets(stringLine, 25, file);
            block[ix][0] = atof(stringLine);
            block[ix][1] = atof(stringLine + 8);
            block[ix][2] = atof(stringLine + 16);
        }
        
        //Calculate distances within a block
        distancesWithinBlock(CurrentBlockLoad, block);

        //Prepare for running the function 'distancesBetweenBlocks'
        if(i > 0)
        {
            size_t PreviousBlock = i;
            for(size_t ix = 0; ix < PreviousBlock; ++ix) //Set parameters
            {
                size_t PreviousBlockStart = firstBlock[ix];
                size_t PreviousBlockRow = numOfRowsInBlock[ix];
                fseek(file, PreviousBlockStart * 24L, SEEK_SET);
                float PreviousBlockValues[ColSize];
                char stringLine[25];
                for(size_t jx = 0; jx < PreviousBlockRow; ++jx)
                {
                    fgets(stringLine, 25, file);
                    PreviousBlockValues[0] = atof(stringLine);
                    PreviousBlockValues[1] = atof(stringLine + 8);
                    PreviousBlockValues[2] = atof(stringLine + 16);
                    
                    //Here we calculate the distance between block    
                    distancesBetweenBlocks(CurrentBlockLoad, block, PreviousBlockValues);
                }
            }
        }

        free(blockEntries);
        free(block);
    } //End of calculating all blocks
    fclose(file);

    size_t i;
    for(i = 0; i < 3465; ++i)
    {   
        printf("%05.2f %ld\n", (double)i/100, count[i]);
    }
    return 0;

}
