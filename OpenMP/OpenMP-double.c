/* Include benchmark-specific header. */
#include "seidel-2d.h"
#include <omp.h>

double bench_t_start, bench_t_end;

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

  	int i, j, threads_per_iteration;
	int numt2 = 4;
	
	int OMP_NUM_THREADS = atoi(getenv ("OMP_NUM_THREADS"));
	int numt = (tsteps > OMP_NUM_THREADS) ? OMP_NUM_THREADS : tsteps;
	omp_set_num_threads(numt);
	printf ("OMP threads: %d\n", numt);

	int iterations = (tsteps % numt ==0) ? (int)(tsteps / numt) : (int)(tsteps / numt + 1);

	omp_set_nested(1);

	#pragma omp parallel private(i,j)
	{

		int k;
		int id = omp_get_thread_num();


		for (k = 0; k < iterations; k++){
			threads_per_iteration = (k + 1 == iterations) ? ((tsteps % numt == 0) ? numt : tsteps % numt) : numt; 
				

			for (i = -(numt2 + 1) * id + 1; i <= n - 2 + (numt2 + 1) * (numt - 1 - id); i += numt2){

				if (i >= 1 && i <= n - 2 && id < threads_per_iteration){
					
					omp_set_num_threads(numt2);

					#pragma omp parallel private(j) firstprivate(i)
					{
						int id2 = omp_get_thread_num();
			      			for (j = (-2) * id2 + 1; j <= n - 2 + 2 * (numt2 - 1 - id2); j++){

							if(j >= 1 && j <= n - 2 && i+id2 <= n - 2)
				  			A[i+id2][j] = (A[i+id2-1][j-1] + A[i+id2-1][j] + A[i+id2-1][j+1] +\
								       A[i+id2][j-1] + A[i+id2][j] + A[i+id2][j+1] +\
								       A[i+id2+1][j-1] + A[i+id2+1][j] + A[i+id2+1][j+1])/9.0f;	
							#pragma omp barrier
						}
					}
				}
				#pragma omp barrier
			}
		}
	}
}

int main(int argc, char** argv)
{
  
  int n = N;
  int tsteps = TSTEPS;

  float (*A)[n][n]; A = (float(*)[n][n])malloc ((n) * (n) * sizeof(float));;

  init_array (n, *A);

  bench_timer_start();;

  kernel_seidel_2d (tsteps, n, *A);

  bench_timer_stop();;
  bench_timer_print();;

  if (argc > 42 && ! strcmp(argv[0], "")) print_array(n, *A);

  free((void*)A);;

  return 0;
}
