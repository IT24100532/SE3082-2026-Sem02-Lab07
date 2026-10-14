Exercise 8: compile and run with make

The Makefile in the repository root compiles all 6 programs with mpicc:
  make          builds sum_bcast, sum_scatter, sum_gather, sum_reduce, sum_allreduce, sum_scan
  make run      runs all 6 with 4 processes (mpirun -np 4)
  make run NP=8 runs them with 8 processes instead
  make clean    removes the programs

The output of "make run" is the output.txt file in each of the Exercise01 to Exercise06
folders.
