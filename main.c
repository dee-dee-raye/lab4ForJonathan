//
//  main.c
//  quicksort
//
//

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "omp.h"

void quickSortParallel(int*, int, int);
void quickSortPartition(int*, int, int);
void quickSort(int*, int, int);

//length of array to sort
int sizeOfArray = 200000000;
int numthreads = 8;

int main(int argc, const char * argv[]) {
    
    int maxNum = sizeOfArray;            //max number to be found in the array
    
    //used for timing the difference between the two sorts
    double time1=0.0;
    double time2=0.0;

    
    //arrays to use for testing
    int* arrayForControl;           //control case array
    int* arrayForStandard;          //standard quicksort array
    int* arrayForParallel;          //parallel sort array
    arrayForControl = (int*) malloc(sizeOfArray*sizeof(int));
    arrayForStandard = (int*) malloc(sizeOfArray*sizeof(int));
    arrayForParallel = (int*) malloc(sizeOfArray*sizeof(int));
    
    
    // Initialise the array with random numbers --------------------------------------
    // variable for the for-loops
    int x;
    //see for random calculations
    srand(5);
    printf("Beginning program.\n\n");
    
    printf("Giving arrays random numbers.\n\n");
    for (x=0; x<sizeOfArray; x++){
        
        arrayForControl[x] = rand() % maxNum + 1;
        arrayForStandard[x] = arrayForControl[x];
        arrayForParallel[x] = arrayForControl[x];
    }
    printf("Arrays are finished and ready to be sorted.\n");
    //--------------------------------------------------------------------------------
    
    //Print out arrays prior to sorting--------(only shows first 50)------------------
        printf("First 50 elements of arrays before sorting.\n");
        for( x = 0 ; x < 50; x++ )
        {
            printf("%d\n", arrayForControl[x]);
        }
        printf("\n\n\n");
    //-------------------------------------------------------------------------------
    
    
    //-------------------------------------------------------------------------------
    printf("Standard Quicksort in progress.\n");
    time1 = clock();
    quickSort(arrayForStandard, 0, sizeOfArray-1);
    time2 = clock();
    //-------------------------------------------------------------------------------
    //Print out results from standard sort-----------(only shows first 50)-----------
    printf("First 50 elements of arrays after standard sorting.\n");
    for(x = 0; x<50; x++)
    {
        printf("%d\n",arrayForStandard[x]);
    }
    printf("Roughly the time spent sorting: %f seconds \n", (float)(time2-time1)/CLOCKS_PER_SEC);
    printf("\n\n\n");
    //-------------------------------------------------------------------------------
    
    
    //-------------------------------------------------------------------------------
    printf("Parallel Quicksort.\n");
    time1 = omp_get_wtime();
    quickSortParallel(arrayForParallel, sizeOfArray, numthreads);
    time2 = omp_get_wtime();
    //-------------------------------------------------------------------------------
    //Print out results from parallel sort------------(only show first 50)-----------
    for(x = 0; x<50; x++)
    {
        printf("%d\n",arrayForParallel[x]);
    }
    printf("Roughly the time spent sorting: %f seconds \n", (time2-time1));
    //-------------------------------------------------------------------------------
   
    return 0;
}

//helper function
void swap(int a[], int i, int j)
{
    int temp;
    temp = a[i];
    a[i] = a[j];
    a[j] = temp;
}

//===============================================================
// quickSortParallel(int* array, int lenArray, int numThreads):
// outside part of sort that basically sets up the parallel for
// the actual sorting in quickSortParallelInside()
// parameters:
// array: array of ints to sort
// sizeOfArray: length of array to sort
// numThreads: number of threads
//===============================================================
void quickSortParallel(int* array, int sizeOfArray, int numThreads)
{
    
    #pragma omp parallel num_threads(numThreads)
    {
        //identifies a section of code that must be run by a
        //single available thread and avoid the implied barrier at
        //the end of the single directive
        #pragma omp single nowait
        {
            quickSortPartition(array, 0, sizeOfArray-1);
        }
    }
    
}


//===============================================================
// quickSortPartition(int* array, int low, int high):
// inner part of quicksort algorithm that uses parallel features
// parameters:
// array: array of ints to sort
// left: index on left side to be sorted
// right: index on right side to be sorted
//===============================================================
void quickSortPartition(int* array, int low, int high)
{
    //set up loop variables
    int lowTemp = low;
    int highTemp = high;
    //center point
    int pivot = array[(low + high) / 2];
    
    
    {
       //this where the partition happens
        while (lowTemp <= highTemp) {
            while (array[lowTemp] < pivot)     //while the left value index hasn't passed the middle pivot point
                lowTemp++;                     //move left side toward pivot (rightward)
            while (array[highTemp] > pivot)    //while right side value index hasn't passed the middle pivot point
                highTemp--;                    //move right side toward pivot (leftward)
            if (lowTemp <= highTemp) {
                swap(array, lowTemp, highTemp);
                lowTemp++;                                  //move left index forward
                highTemp--;                                 //move right index forward
            }
        }
        
    }
    
    
    if ( ((high-low) < 700) )         //if the right is not as far from the left as needed (the partition isn't that big)
    {                                //just continue serially otherwise you get errors
        if (low < highTemp)
        {
            quickSortPartition(array, low, highTemp);
        }
        if (lowTemp < high)
        {
            quickSortPartition(array, lowTemp, high);

        }
        
    }
    else
    {
        // omp task - identify a block of code to be executed in parallel with the code outside the task
        // partition both sides
        #pragma omp task
        {
            quickSortPartition(array, low, highTemp);
            
        }
        #pragma omp task
        {
            quickSortPartition(array, lowTemp, high);
            
        }
    }
    
}



//===============================================================
// quickSort(int* array, int low, int high): basic quick sort
// algorithm that useses recurrsion found at:
// http://www.bogotobogo.com/Algorithms/quicksort.php
// parameters:
// array: array of ints to sort
// low: index on left side to be sorted
// high: index on right side to be sorted
//===============================================================
void quickSort(int* a, int left_index, int right_index)
{
    int left;
    int right;
    int pivot;
    
    if(left_index >= right_index)
        return;
    
    left = left_index;
    right = right_index;
    
    // pivot selection
    pivot = a[(left_index + right_index) /2];
    
    // partition
    while(left <= right) {
        while(a[left] < pivot)
            left++;
        while(a[right] > pivot)
            right--;
        if(left <= right) {
            swap(a,left,right);
            left++;
            right--;
        }
    }
    
    // recursion
    quickSort(a,left_index,right);
    quickSort(a,left,right_index);
}

