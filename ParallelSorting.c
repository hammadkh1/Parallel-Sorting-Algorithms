#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

// Function declarations
void parallelBucketSort(int *data, int size, int range);
void merge(int *data, int left, int mid, int right);
void quicksort(int *data, int left, int right);
void insertionSort(int *arr, int n);
void mergeParallel(int *data, int left, int right); // Declaration added
void quicksortParallel(int *data, int left, int right); // Declaration added
void parallelMergeSort(int *data, int size);
void parallelQuickSort(int *data, int size);

// Function to print an array
void printArray(int *data, int size) {
    for (int i = 0; i < size; ++i) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

// Function to swap two integers
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Function to perform insertion sort within each bucket
void insertionSort(int *arr, int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }

        arr[j + 1] = key;
    }
}

// Function to partition the array for quicksort
int partition(int *data, int left, int right) {
    int pivot = data[right];
    int i = left - 1;

    for (int j = left; j < right; ++j) {
        if (data[j] <= pivot) {
            i++;
            swap(&data[i], &data[j]);
        }
    }

    swap(&data[i + 1], &data[right]);
    return i + 1;
}

// Serial quicksort function
void quicksort(int *data, int left, int right) {
    if (left < right) {
        int pivotIndex = partition(data, left, right);
        quicksort(data, left, pivotIndex - 1);
        quicksort(data, pivotIndex + 1, right);
    }
}

// Merge function for merge sort
void merge(int *data, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int *leftArr = (int *)malloc(n1 * sizeof(int));
    int *rightArr = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; ++i)
        leftArr[i] = data[left + i];
    for (int j = 0; j < n2; ++j)
        rightArr[j] = data[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            data[k] = leftArr[i];
            i++;
        } else {
            data[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        data[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2) {
        data[k] = rightArr[j];
        j++;
        k++;
    }

    free(leftArr);
    free(rightArr);
}

// Parallel Merge Sort implementation
void parallelMergeSort(int *data, int size) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        mergeParallel(data, 0, size - 1);
    }
}

// Merge function for Merge Sort (Parallel version)
void mergeParallel(int *data, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        #pragma omp task
        mergeParallel(data, left, mid);

        #pragma omp task
        mergeParallel(data, mid + 1, right);

        merge(data, left, mid, right);
    }
}

// Parallel Quick Sort implementation
void parallelQuickSort(int *data, int size) {
    #pragma omp parallel
    {
        #pragma omp single nowait
        quicksortParallel(data, 0, size - 1);
    }
}

// Quicksort helper function (Parallel version)
void quicksortParallel(int *data, int left, int right) {
    if (left < right) {
        int pivotIndex = partition(data, left, right);

        #pragma omp task
        quicksortParallel(data, left, pivotIndex - 1);

        #pragma omp task
        quicksortParallel(data, pivotIndex + 1, right);
    }
}

// Parallel Bucket Sort implementation
void parallelBucketSort(int *data, int size, int range) {
    #pragma omp parallel
    {
        int *localData = (int *)malloc(size * sizeof(int));
        int localSize;

        #pragma omp for
        for (int i = 0; i < size; ++i) {
            localData[i] = data[i];
        }

        #pragma omp single
        {
            localSize = size;
        }

        // Sort local data using insertion sort
        insertionSort(localData, localSize);

        // Gather sorted local data to the root process
        #pragma omp critical
        {
            MPI_Gather(localData, localSize, MPI_INT, data, localSize, MPI_INT, 0, MPI_COMM_WORLD);
        }

        free(localData);
    }
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int choice, range, n;

    if (rank == 0) {
        printf("Choose the sorting algorithm:\n");
        printf("1. Parallel Bucket Sort\n");
        printf("2. Parallel Merge Sort\n");
        printf("3. Parallel Quick Sort\n");

        scanf("%d", &choice);

        printf("Enter the number range: ");
        scanf("%d", &range);

        printf("Enter the number of elements: ");
        scanf("%d", &n);
    }

    MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&range, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *data = (int *)malloc(n * sizeof(int));

    if (rank == 0) {
        printf("Enter %d numbers:\n", n);
        for (int i = 0; i < n; ++i) {
            scanf("%d", &data[i]);
        }
    }

    MPI_Bcast(data, n, MPI_INT, 0, MPI_COMM_WORLD);

    switch (choice) {
        case 1:
            parallelBucketSort(data, n, range);
            break;
        case 2:
            parallelMergeSort(data, n);
            break;
        case 3:
            parallelQuickSort(data, n);
            break;
        default:
            if (rank == 0) {
                printf("Invalid choice.\n");
            }
    }

    if (rank == 0) {
        printf("Sorted array:\n");
        printArray(data, n);
    }

    free(data);

    MPI_Finalize();
    return 0;
}