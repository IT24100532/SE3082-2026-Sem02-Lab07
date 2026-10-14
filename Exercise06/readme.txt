Exercise 6: prefix sums with Scan (sum_scan.c)
Run on: Debian Linux container (Docker), Open MPI 5.0.7, gcc 14.2, 8 cpus.

Changes from Exercise 5:
- MPI_Scan(&local_sum, &prefix_sum, 1, MPI_LONG_LONG, MPI_SUM, MPI_COMM_WORLD)
  gives each rank the sum of all chunks from rank 0 up to and including itself.
- Each rank prints local_sum, prefix_sum and sum_before_me = prefix_sum - local_sum.
- Verification bonus: each rank checks prefix_sum against K(K+1)/2 with
  K = (rank + 1) * chunk_size. All ranks print OK.
- The last rank checks that its prefix_sum is the global total.

Output (4 processes):
Root filled array with values 1 to 1000000
  Rank 0: local_sum = 31250125000, prefix_sum = 31250125000, sum_before_me = 0, check K(K+1)/2: OK
  Rank 1: local_sum = 93750125000, prefix_sum = 125000250000, sum_before_me = 31250125000, check K(K+1)/2: OK
  Rank 2: local_sum = 156250125000, prefix_sum = 281250375000, sum_before_me = 125000250000, check K(K+1)/2: OK
  Rank 3: local_sum = 218750125000, prefix_sum = 500000500000, sum_before_me = 281250375000, check K(K+1)/2: OK

[Scan] Last rank prefix_sum = 500000500000
[Scan] Expected             = 500000500000
[Scan] Correct?             = YES
[Scan] Time                 = 0.0028 sec

Rank 0's sum_before_me is 0, and each later rank's sum_before_me is the previous rank's
prefix_sum. That value is the global offset of the rank's chunk.
