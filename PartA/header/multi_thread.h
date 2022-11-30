#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

struct parameters {
	int *matrix_A;
	int *matrix_B;
	int *matrix_output;
	int start;
	int end;
	int N;
};

void *func(void *ptr) {
	struct parameters *params = (struct parameters *) ptr;
	int *matrix_A = params->matrix_A;
	int *matrix_B = params->matrix_B;
	int *matrix_output = params->matrix_output;
	int start = params->start;
	int end = params->end;
	int N = params->N;
	
	for (int j = start; j < end; j += 2) {
		int col_1[N];
		int col_2[N];

		for (int k = 0; k < N; k++) {
			col_1[k] = matrix_B[(k*N) + j];
			col_2[k] = matrix_B[(k*N) + j+1];
		} 

		for (int i = 0; i < N; i += 2) {
			int sum = 0;

			for (int k = 0; k < N; k++) {
				sum += matrix_A[(i*N)+k] * col_1[k];
				sum += matrix_A[(i*N)+k] * col_2[k];
				sum += matrix_A[(i+1)*N+k] * col_1[k];
				sum += matrix_A[(i+1)*N+k] * col_2[k];
			}

			int r = i >> 1;
			int offset = j >> 1;
			
			matrix_output[(r*(N>>1)) + offset] = sum; 
		}
	}
}

void multiThread(int N, int *matrix_A, int *matrix_B, int *matrix_output) {
	int num_threads = 8;
	int rows_per_thread = N / num_threads;

	pthread_t *threads = (pthread_t *) malloc(num_threads * sizeof(pthread_t));
	struct parameters *params = (struct parameters*) malloc(num_threads * sizeof(pthread_t));

	int k = 0;
	for (int i = 0; i < num_threads; i++) {
		params[i].matrix_A = matrix_A;
		params[i].matrix_B = matrix_B;
		params[i].matrix_output = matrix_output;
		params[i].start = k;
		params[i].end = k + rows_per_thread - 1;
		params[i].N = N;
		k += rows_per_thread;
	}

	for (int i = 0; i < num_threads; i++) {
		pthread_create(&threads[i], NULL, func, (void *)&params[i]);
	}

	for (int i = 0; i < num_threads; i++) {
		pthread_join(threads[i], NULL );
	}    
}
