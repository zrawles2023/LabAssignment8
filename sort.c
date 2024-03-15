#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int extraMemoryAllocated;

void *Alloc(size_t sz)
{
	extraMemoryAllocated += sz;
	size_t* ret = malloc(sizeof(size_t) + sz);
	*ret = sz;
	printf("Extra memory allocated, size: %ld\n", sz);
	return &ret[1];
}

void DeAlloc(void* ptr)
{
	size_t* pSz = (size_t*)ptr - 1;
	extraMemoryAllocated -= *pSz;
	printf("Extra memory deallocated, size: %ld\n", *pSz);
	free((size_t*)ptr - 1);
}

size_t Size(void* ptr)
{
	return ((size_t*)ptr)[-1];
}

// implement merge sort
// extraMemoryAllocated counts bytes of extra memory allocated
void mergeSort(int pData[], int l, int r)
{
    // base case
    if (l < r) 
	{
        // middle index calc
        int m = l + (r - l) / 2;
        
        // sort left side and right side arrays 
        mergeSort(pData, l, m);
        mergeSort(pData, m + 1, r);

        // now merge the arrays
        int n1 = m - l + 1;
        int n2 = r - m;

        // dma for temp arrays with merge sort algos
        int *L = (int *)Alloc(n1 * sizeof(int));
        int *R = (int *)Alloc(n2 * sizeof(int));

        // copy the data to these arrays
        memcpy(L, &pData[l], n1*sizeof(int));
        memcpy(R, &pData[m + 1], n2*sizeof(int));

        // merge to arrays to pData
        int i = 0, j = 0, k = l;
        while (i < n1 && j < n2) 
		{
            if (L[i] <= R[j]) 
			{
                pData[k] = L[i];
                i++;
            } else 
			{
                pData[k] = R[j];
                j++;
            }
            k++;
        }

        // copy anyting left in L or R
        while (i < n1)
		{
            pData[k] = L[i];
            i++;
            k++;
        }

        while (j < n2) 
		{
            pData[k] = R[j];
            j++;
            k++;
        }

        // dealloc L and R
        DeAlloc(L);
        DeAlloc(R);
    }
}



// parses input file to an integer array
int parseData(char *inputFileName, int **ppData)
{
	FILE* inFile = fopen(inputFileName,"r");
	int dataSz = 0;
	int i, n, *data;
	*ppData = NULL;
	
	if (inFile)
	{
		fscanf(inFile,"%d\n",&dataSz);
		*ppData = (int *)malloc(sizeof(int) * dataSz);
		// Implement parse data block
		if (*ppData == NULL)
		{
			printf("Cannot allocate memory\n");
			exit(-1);
		}
		for (i=0;i<dataSz;++i)
		{
			fscanf(inFile, "%d ",&n);
			data = *ppData + i;
			*data = n;
		}

		fclose(inFile);
	}
	
	return dataSz;
}

// prints first and last 100 items in the data array
void printArray(int pData[], int dataSz)
{
    int i, sz = (dataSz > 100 ? dataSz - 100 : 0);
    int firstHundred = (dataSz < 100 ? dataSz : 100);
    printf("\tData:\n\t");
    for (i=0;i<firstHundred;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\t");
    
    for (i=sz;i<dataSz;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\n");
}

int main(void)
{
	clock_t start, end;
	int i;
    double cpu_time_used;
	char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt", "input4.txt" };
	
	for (i=0;i<4;++i)
	{
		int *pDataSrc, *pDataCopy;
		int dataSz = parseData(fileNames[i], &pDataSrc);
		
		if (dataSz <= 0)
			continue;
		
		pDataCopy = (int *)malloc(sizeof(int)*dataSz);
	
		printf("---------------------------\n");
		printf("Dataset Size : %d\n",dataSz);
		printf("---------------------------\n");
		
		printf("Merge Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz*sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		mergeSort(pDataCopy, 0, dataSz - 1);
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n",cpu_time_used);
		printf("\textra memory allocated\t: %d\n",extraMemoryAllocated);
		printArray(pDataCopy, dataSz);
		
		free(pDataCopy);
		free(pDataSrc);
	}
	
}