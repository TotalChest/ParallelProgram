# Program-Parallelization
Parallelizing program with OpneMP and MPI on BlueGene and Polus supercomputers    
You can run OpenMp programs on your computer. The program itself displays the execution time of the algorithm
### Sequence:
```bash
$ gcc original.c -o original
$ ./original
```
### Parallel:
```bash
$ export OMP_NUM_THREADS=$1    # $1 = 1/2/4/8/32/64/128  
$ gcc OpenMP.c -o first -fopenmp
$ gcc OpenMP-second.c -o second -fopenmp
$ gcc OpenMP-double.c -o double -fopenmp
$ ./first
$ ./second
$ ./double
```
