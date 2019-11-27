# Program-Parallelization
Parallelizing program with OpneMP and MPI on BlueGene and Polus supercomputers  
### Sequence:
> gcc seidel-2d-sequence.c -o Sequence  
> ./Sequence 2> /dev/null  
### Sequence:
> export OMP_NUM_THREADS=$1    # $1 = 2/4/8/32/64  
> gcc seidel-2d-parallel.c -o Parallel -fopenmp  
> ./Parallel 2> /dev/null  
