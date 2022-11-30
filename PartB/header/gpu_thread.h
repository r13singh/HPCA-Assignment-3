#include <stdio.h>
#include <cuda.h>
#include <stdlib.h>
#include <time.h>
#include <cuda_runtime.h>

_global_ void cuda_RMM(int *matrix_A, int *matrix_B, int *matrix_output) {
	int row_A = blockIdx.y * blockDim.y + threadIdx.y;
	int col_B = blockIdx.x * blockDim.x + threadIdx.x;

	if (((row_A % 2) == 0) && (row_A < N) && (col_B + 1 < N)) {
		int sum = 0;
		
		for(int index = 0; index < N; index++) {
			sum += matrix_A[row_A * N + index] * matrix_B[index * N + col_B];
			sum += matrix_A[(row_A+1) * N + index] * matrix_B[index * N + col_B];
			sum += matrix_A[row_A * N + index] * matrix_B[index * N + (col_B+1)];
			sum += matrix_A[(row_A+1) * N + index] * matrix_B[index * N + (col_B+1)];
		}

		int row_C = row_A >> 1;
		int col_C = col_B >> 1;

		int k = row_C * (N>>1) + col_C;
		matrix_output[k] = sum;
	}
}


void gpuThread(int N, int *matrix_A, int *matrix_B, int* matrix_output) {  
	int num_thread = 32;
	int num_block = N / num_thread;

	dim3 threads(num_thread, num_thread);
	dim3 blocks(num_block, num_block);

	int *matrix_A_dev, *matrix_B_dev, *matrix_output_dev;

	// Allocate device memory
	cudaMalloc((void**)&matrix_A_dev, N*N*sizeof(int));
	cudaMalloc((void**)&matrix_B_dev, N*N*sizeof(int));
	cudaMalloc((void**)&matrix_output_dev, N*N/4*sizeof(int));

	// Copy data to the device
	cudaMemcpy(matrix_A_dev, matrix_A, total*sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(matrix_B_dev, matrix_B, total*sizeof(int), cudaMemcpyHostToDevice);

	// Launch kernel
	cuda_RMM<<<blocks, threads>>>(matrix_A_dev,matrix_B_dev,matrix_output_dev);

	// Copy back to the host  
	cudaMemcpy(matrix_output, matrix_output_dev, N*N/4*sizeof(int), cudaMemcpyDeviceToHost);

	// Free memory on device
	cudaFree(dev_a);
	cudaFree(dev_b);
	cudaFree(dev_c);
}
