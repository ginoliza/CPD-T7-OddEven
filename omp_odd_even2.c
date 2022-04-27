/* Archivo:    omp_odd_even2.c
 *
 * Proposito: Usar el odd even transposition sort para ordenar una lista de enteros
 *
 * Compilar: gcc -g -Wall -fopenmp -I. -o omp_odd_even2 omp_odd_even2.c
 * Usar:   ./omp_odd_even2 <thread count> <n> <g|i>
 *             n:   numero de elementos en la lista
 *            'g':  generar la lista aleatoriamente
 *            'i':  lista de entrada de usuario
 *
 * Entdada:   lista (opcional)
 * Salida:  tiempo que demora del algoritmo
 *
 * Notas:
 * 1.  -DEBUG imprime el contenido de la lista
 * 2.  Esta version bifurca y une los threads solo una vez
 * 3.  Usa la funcion de OpenMP omp_get_wtime para medir tiempo en segundos.
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#ifdef DEBUG
const int RMAX = 100;
#else
const int RMAX = 10000000;
#endif

int thread_count;

void Usage(char* prog_name);
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p);
void Generate_list(int a[], int n);
void Print_list(int a[], int n, char* title);
void Read_list(int a[], int n);
void Odd_even(int a[], int n);

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int  n;
   char g_i;
   int* a;
   double start, finish;

   Get_args(argc, argv, &n, &g_i);
   a = malloc(n*sizeof(int));
   if (g_i == 'g') {
      Generate_list(a, n);
#     ifdef DEBUG
      Print_list(a, n, "Before sort");
#     endif
   } else {
      Read_list(a, n);
   }

   start = omp_get_wtime();
   Odd_even(a, n);
   finish = omp_get_wtime();

#  ifdef DEBUG
   Print_list(a, n, "After sort");
#  endif
   
   printf("Elapsed time = %e seconds\n", finish - start);

   free(a);
   return 0;
}  /* main */


/*-----------------------------------------------------------------
 * Funcion:  Usage
 * Proposito:   Como correr el programa
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage:   %s <thread count> <n> <g|i>\n", prog_name);
   fprintf(stderr, "   n:   number of elements in list\n");
   fprintf(stderr, "  'g':  generate list using a random number generator\n");
   fprintf(stderr, "  'i':  user input list\n");
}  /* Usage */


/*-----------------------------------------------------------------
 * Funcion:  Get_args
 * Proposito:   Obtener y verificar argumentos de terminal
 * args de entrada:   argc, argv
 * args de salida:  n_p, g_i_p
 */
void Get_args(int argc, char* argv[], int* n_p, char* g_i_p) {
   if (argc != 4 ) {
      Usage(argv[0]);
      exit(0);
   }
   thread_count = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
   *g_i_p = argv[3][0];

   if (*n_p <= 0 || (*g_i_p != 'g' && *g_i_p != 'i') ) {
      Usage(argv[0]);
      exit(0);
   }
}  /* Get_args */


/*-----------------------------------------------------------------
 * Funcion:  Generate_list 
 * Proposito:   Usar un generador de numeros aleatorios para generar los elementos de la lista
 * args de entrada:   n
 * args de salida:  a
 */
void Generate_list(int a[], int n) {
   int i;

   srandom(1);
   for (i = 0; i < n; i++)
      a[i] = random() % RMAX;
}  /* Generate_list */


/*-----------------------------------------------------------------
 * Funcion:  Print_list
 * Proposito:   Imprimir los elementos en la lista
 * args de entrada:   a, n
 */
void Print_list(int a[], int n, char* title) {
   int i;

   printf("%s:\n", title);
   for (i = 0; i < n; i++)
      printf("%d ", a[i]);
   printf("\n\n");
}  /* Print_list */


/*-----------------------------------------------------------------
 * Funcion:  Read_list
 * Proposito:   Leer los elementos de la lista de stdin
 * args de entrada:   n
 * args de salida:  a
 */
void Read_list(int a[], int n) {
   int i;

   printf("Please enter the elements of the list\n");
   for (i = 0; i < n; i++)
      scanf("%d", &a[i]);
}  /* Read_list */


/*-----------------------------------------------------------------
 * Funcion:     Odd_even
 * Proposito:      Ordenar la lista con el algoritmo odd-even transposition sort
 * args de entrada:      n
 * args de entrada/salida:  a
 */
void Odd_even(int a[], int n) {
   int phase, i, tmp;

#  pragma omp parallel num_threads(thread_count) \
      default(none) shared(a, n) private(i, tmp, phase)
   for (phase = 0; phase < n; phase++) {
      if (phase % 2 == 0)
#        pragma omp for 
         for (i = 1; i < n; i += 2) {
            if (a[i-1] > a[i]) {
               tmp = a[i-1];
               a[i-1] = a[i];
               a[i] = tmp;
            }
         }
      else
#        pragma omp for 
         for (i = 1; i < n-1; i += 2) {
            if (a[i] > a[i+1]) {
               tmp = a[i+1];
               a[i+1] = a[i];
               a[i] = tmp;
            }
         }
   }
}  /* Odd_even */

