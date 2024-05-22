# Parallel Sorting Algorithms with MPI and OpenMP

This repository contains C code implementing parallel sorting algorithms using MPI (Message Passing Interface) and OpenMP (Open Multi-Processing). The implemented sorting algorithms are:

1. Parallel Bucket Sort
2. Parallel Merge Sort
3. Parallel Quick Sort

## Compilation

To compile the code, you need an MPI implementation and an OpenMP compiler. Here's an example compilation command using `mpicc`:

```
mpicc -o parallel_sort parallel_sort.c -fopenmp
```

## Running

To run the program, use the following command:

```
mpirun -np <number_of_processes> ./parallel_sort
```

Replace `<number_of_processes>` with the desired number of MPI processes.

## Usage

Upon running the program, you will be prompted to choose a sorting algorithm and provide necessary inputs such as the number range and the number of elements to sort. Here's the usage flow:

1. Choose the sorting algorithm:
    - Enter `1` for Parallel Bucket Sort.
    - Enter `2` for Parallel Merge Sort.
    - Enter `3` for Parallel Quick Sort.

2. Enter the number range: This defines the range of numbers to be sorted.

3. Enter the number of elements: Specify the number of elements you want to sort.

4. Enter the numbers: Input the numbers you want to sort. Repeat this step for each element until all elements are entered.

After entering the required inputs, the program will sort the provided numbers using the selected algorithm and display the sorted array.

## Implementation Details

- **Parallel Bucket Sort**: This algorithm sorts the numbers using the bucket sort algorithm with OpenMP for parallelization.
- **Parallel Merge Sort**: The merge sort algorithm is parallelized using OpenMP tasks for dividing and merging subarrays.
- **Parallel Quick Sort**: Quick sort is parallelized using OpenMP tasks for recursive calls and partitioning.
