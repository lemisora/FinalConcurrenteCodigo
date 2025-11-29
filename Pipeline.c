#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>

#define TAG_DATA 0
#define TAG_END 1

int main(int argc, char** argv) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int almacenado = 0;
    int tiene_valor = 0; 
    int dato_entrante;
    MPI_Status status;

    // ---------------------------------------------------------
    // PROCESO MAESTRO (Rank 0)
    // ---------------------------------------------------------
    if (rank == 0) {
        int n_etapas = size - 1; 
        
        // Verificamos si el usuario pasó números al ejecutar
        // argc incluye el nombre del programa, así que restamos 1
        int n_entradas = argc - 1; 

        printf("-------------------------------------------------\n");
        printf(" [MAESTRO] Pipeline Sort con %d procesos (Capacidad: %d)\n", size, n_etapas);
        printf("-------------------------------------------------\n");

        if (n_entradas < 1) {
            fprintf(stderr, "[ERROR] No has introducido numeros.\n");
            fprintf(stderr, "Uso correcto: mpirun -np 6 ./pipeline 5 10 2 8 1\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (n_entradas > n_etapas) {
            fprintf(stderr, "[ERROR] Intentas ordenar %d numeros pero solo tienes %d etapas (procesos).\n", n_entradas, n_etapas);
            fprintf(stderr, "Incrementa el -np o reduce los numeros.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        printf(" [MAESTRO] Ordenando la secuencia: ");
        for (int i=1; i<=n_entradas; i++) printf("%s ", argv[i]);
        printf("\n\n");

        // 1. Inyectar datos (leídos directamente de argv)
        for (int i = 1; i <= n_entradas; i++) {
            int num = atoi(argv[i]); // Convertir texto a entero
            MPI_Send(&num, 1, MPI_INT, 1, TAG_DATA, MPI_COMM_WORLD);
        }

        // 2. Etapa de descarga (Push con INT_MAX)
        for (int i = 0; i < size; i++) {
             int descarga = INT_MAX;
             MPI_Send(&descarga, 1, MPI_INT, 1, TAG_DATA, MPI_COMM_WORLD);
        }

        // 3. Señal de terminación
        MPI_Send(0, 0, MPI_INT, 1, TAG_END, MPI_COMM_WORLD);
        
    // ---------------------------------------------------------
    // PROCESOS DE ETAPA (Rank 1..N)
    // ---------------------------------------------------------
    } else {
        int terminado = 0;
        while (!terminado) {
            MPI_Recv(&dato_entrante, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == TAG_END) {
                if (rank < size - 1) {
                    MPI_Send(0, 0, MPI_INT, rank + 1, TAG_END, MPI_COMM_WORLD);
                }
                terminado = 1; 
            } else {
                if (!tiene_valor) {
                    almacenado = dato_entrante;
                    tiene_valor = 1;
                } else {
                    int a_enviar;
                    if (dato_entrante < almacenado) {
                        a_enviar = dato_entrante;    // Pasa el menor
                    } else {
                        a_enviar = almacenado;       // Pasa el que tenía (menor)
                        almacenado = dato_entrante;  // Guardo el mayor
                    }

                    if (rank < size - 1) {
                        MPI_Send(&a_enviar, 1, MPI_INT, rank + 1, TAG_DATA, MPI_COMM_WORLD);
                    } else {
                        // Último nodo: Imprimir resultado
                        if (a_enviar != INT_MAX) {
                            printf(" -> [SALIDA] %d\n", a_enviar);
                        }
                    }
                }
            }
        }
    }

    MPI_Finalize();
    return 0;
}