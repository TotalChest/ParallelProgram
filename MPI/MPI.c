/* Include benchmark-specific header. */
#include "seidel-2d.h"
#include <mpi.h>

double bench_t_start, bench_t_end;

int ranksize, id;
MPI_Status status;

static
double rtclock()
{
    struct timeval Tp;
    int stat;
    stat = gettimeofday (&Tp, NULL);
    if (stat != 0)
      printf ("Error return from gettimeofday: %d", stat);
    return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
}

void bench_timer_start()
{
  bench_t_start = rtclock ();
}

void bench_timer_stop()
{
  bench_t_end = rtclock ();
}

void bench_timer_print()
{
  printf ("Time in seconds = %0.6lf\n", bench_t_end - bench_t_start);
}

static
void init_array (int n,
   float A[ n][n])
{
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      A[i][j] = ((float) i*(j+2) + 2) / n;
}

static
void print_array(int n,
   float A[ n][n])
{
  int i, j;

  fprintf(stderr, "==BEGIN DUMP_ARRAYS==\n");
  fprintf(stderr, "begin dump: %s", "A");
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      if ((i * n + j) % 20 == 0) fprintf(stderr, "\n");
      fprintf(stderr, "%0.2f ", A[i][j]);
    }
  fprintf(stderr, "\nend   dump: %s\n", "A");
  fprintf(stderr, "==END   DUMP_ARRAYS==\n");
}

static
void kernel_seidel_2d(int tsteps,
        int n,
        float A[ n][n])
{

	int i, j, k, process_per_iteration;

	MPI_Comm_size(MPI_COMM_WORLD, &ranksize);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Barrier(MPI_COMM_WORLD);

	int iterations = (tsteps % ranksize ==0) ? (int)(tsteps / ranksize) : (int)(tsteps / ranksize + 1);

	for (k = 0; k < iterations; k++){
		process_per_iteration = (k + 1 == iterations) ? ((tsteps % ranksize == 0) ? ranksize : tsteps % ranksize) : ranksize;

		if (id == 0 && k != 0)
			MPI_Recv(&A[0][0], n * n, MPI_FLOAT, ranksize - 1, 1216, MPI_COMM_WORLD, &status);

		for (i = (-2) * id + 1; i <= n - 2 + 2 * (process_per_iteration - 1 - id); i++){

			if (id != 0 && i >= 0 && i <= n - 3 && id < process_per_iteration)
				MPI_Recv(&A[i+1][1], n - 2, MPI_FLOAT, id - 1, 1215, MPI_COMM_WORLD, &status);
		
			if (i >= 1 && i <= n - 2 && id < process_per_iteration){

				for (j = 1; j <= n - 2; j++)
					
			  		A[i][j] = (A[i-1][j-1] + A[i-1][j] + A[i-1][j+1] + A[i][j-1] + A[i][j] + A[i][j+1] + A[i+1][j-1] + A[i+1][j] + A[i+1][j+1])/9.0f;

			  	if (id != process_per_iteration - 1)
					MPI_Send(&A[i][1], n - 2, MPI_FLOAT, id + 1, 1215, MPI_COMM_WORLD);
			}

		}

		if (id == ranksize - 1 && k + 1 != iterations)
			MPI_Send(&A[0][0], n * n, MPI_FLOAT, 0, 1216, MPI_COMM_WORLD);
	}
	if (id == 0)
			MPI_Recv(&A[0][0], n * n, MPI_FLOAT, process_per_iteration - 1, 1217, MPI_COMM_WORLD, &status);
	if (id == process_per_iteration - 1)
			MPI_Send(&A[0][0], n * n, MPI_FLOAT, 0, 1217, MPI_COMM_WORLD);
}

int main(int argc, char** argv)
{
  int n = N;
  int tsteps = TSTEPS;

  MPI_Init(&argc, &argv);

  float (*A)[n][n]; A = (float(*)[n][n])malloc ((n) * (n) * sizeof(float));;

  init_array (n, *A);

  bench_timer_start();;

  kernel_seidel_2d (tsteps, n, *A);

  bench_timer_stop();;
  bench_timer_print();;

  if (argc > 42 && ! strcmp(argv[0], "")) print_array(n, *A);

  free((void*)A);;

  MPI_Finalize();

  return 0;
}
