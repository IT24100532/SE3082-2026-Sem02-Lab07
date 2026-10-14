Exercise 7: summary comparison
Run on: Debian Linux container (Docker), Open MPI 5.0.7, gcc 14.2, 8 cpus.

1. Comparison of the 6 programs

Program          Collectives used           Full array or chunk?        Manual loop on root?   Result available on
sum_bcast        Bcast + Send/Recv          every process: full array   yes (Recv loop)        root only
sum_scatter      Scatter + Send/Recv        root: full, others: chunk   yes (Recv loop)        root only
sum_gather       Scatter + Gather           root: full, others: chunk   yes (adds all_sums)    root only
sum_reduce       Scatter + Reduce           root: full, others: chunk   no                     root only
sum_allreduce    Scatter + Allreduce        root: full, others: chunk   no                     all processes
sum_scan         Scatter + Scan             root: full, others: chunk   no                     different per rank
                                                                                               (prefix sums)

2. Times (seconds, median of 5 runs, measured by MPI_Wtime on the rank that prints)

Program          2 processes   4 processes   8 processes
sum_bcast        0.0033        0.0085        0.0173
sum_scatter      0.0020        0.0015        0.0022
sum_gather       0.0021        0.0015        0.0020
sum_reduce       0.0021        0.0016        0.0023
sum_allreduce    0.0020        0.0015        0.0106
sum_scan         0.0035        0.0047        0.0109

Fastest: Scatter, Gather and Reduce (about 1.5 ms with 4 processes).
Why:
- Bcast is the slowest and gets slower with more processes, because the whole 4 MB array
  is copied to every process: with 8 processes that is 8 x 4 MB, although each process
  needs only 1/8 of it. Scatter sends each process only its own part.
- Gather and Reduce are about as fast as the Send/Recv loop here: they collect only one
  number per process, which is tiny, so the collection step costs almost nothing compared
  with distributing the array. Reduce has the cleanest code (no loop on root).
- Allreduce and Scan are as fast as Reduce with 2 and 4 processes, but slower and much
  noisier with 8 (the runs range from 0.003 to 0.038 s). With 8 processes on 8 cpus there
  is no free core, and these two operations make every process wait for the others, so
  any delay on one process slows down all of them. On a real cluster with spare cores the
  difference would be much smaller.
- All times are only a few milliseconds and change between runs, so small differences
  (less than about 0.001 s) are not meaningful.

3. When would I choose MPI_Scan over MPI_Allreduce?
When each process needs the total of everything BEFORE it, not the grand total.
Example: each process has filtered some records and keeps a different number of them
(rank 0 keeps 120, rank 1 keeps 80, rank 2 keeps 200). To write all of them into one
output file without overlapping, every process must know where its records start.
MPI_Scan on the counts gives 120, 200, 400, and prefix - own count gives the start
positions 0, 120 and 200. Allreduce would only give every process the total (400).
