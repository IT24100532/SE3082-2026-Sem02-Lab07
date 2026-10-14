// Exercise 6: Allreduce replaced with Scan. Each rank gets the prefix sum of all
// chunks from rank 0 up to and including itself, so every rank gets a different value.
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000000

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int chunk_size = N / size;
    int *array = NULL;
    int *local_chunk = (int *)malloc(chunk_size * sizeof(int));

    if (rank == 0) {
        array = (int *)malloc(N * sizeof(int));
        for (int i = 0; i < N; i++)
            array[i] = i + 1;
        printf("Root filled array with values 1 to %d\n", N);
    }

    double start = MPI_Wtime();

    MPI_Scatter(array, chunk_size, MPI_INT, local_chunk, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    long long local_sum = 0;
    for (int i = 0; i < chunk_size; i++)
        local_sum += local_chunk[i];

    /* SCAN: prefix_sum = local_sum of rank 0 + ... + local_sum of this rank */
    long long prefix_sum = 0;
    MPI_Scan(&local_sum, &prefix_sum, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD);

    double elapsed = MPI_Wtime() - start;

    long long sum_before_me = prefix_sum - local_sum;   /* global offset of this chunk */
    long long K = (long long)(rank + 1) * chunk_size;   /* last value in this chunk */
    long long formula = K * (K + 1) / 2;                /* 1 + 2 + ... + K */
    printf("  Rank %d: local_sum = %lld, prefix_sum = %lld, sum_before_me = %lld, check K(K+1)/2: %s\n",
           rank, local_sum, prefix_sum, sum_before_me, prefix_sum == formula ? "OK" : "WRONG");

    if (rank == size - 1) {   /* the last rank's prefix sum is the global total */
        long long expected = (long long)N * (N + 1) / 2;
        printf("\n[Scan] Last rank prefix_sum = %lld\n", prefix_sum);
        printf("[Scan] Expected             = %lld\n", expected);
        printf("[Scan] Correct?             = %s\n", prefix_sum == expected ? "YES" : "NO");
        printf("[Scan] Time                 = %.4f sec\n", elapsed);
    }

    if (rank == 0) free(array);
    free(local_chunk);
    MPI_Finalize();
    return 0;
}
