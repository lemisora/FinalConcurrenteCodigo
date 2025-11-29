/*
 * MPI: anillo.c
 * Transmisión de un vector en anillo (Ring Topology)
 * Objetivo: Medir ancho de banda y latencia.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    int pid, npr;
    int siguiente, anterior;
    int *vector = NULL;
    int lgmax, nvueltas, tam;
    int i, v, k;
    double t0, t1;
    MPI_Status info;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &npr);

    // Definir vecinos
    siguiente = (pid + 1) % npr;
    anterior = (pid + npr - 1) % npr;

    // Lectura de parámetros (solo proceso 0)
    if (pid == 0) {
        // Valores definidos en la práctica
        lgmax = 20; // Hasta 2^20 enteros
        nvueltas = 4; // Número de vueltas al anillo
        printf("Configuracion: Vueltas=%d, PotenciaMax=%d\n", nvueltas, lgmax);
    }

    // Distribuir configuración a todos
    MPI_Bcast(&lgmax, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nvueltas, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Bucle para diferentes tamaños de mensaje (Potencias de 2)
    for (k = 0; k <= lgmax; k++) {
        tam = (int)pow(2, k);

        // Reservar memoria
        vector = (int*) malloc(sizeof(int) * tam);

        // Inicializar vector
        if (pid == 0) {
            for(i=0; i<tam; i++) vector[i] = 1;
        }

        MPI_Barrier(MPI_COMM_WORLD); // Sincronizar antes de medir
        if (pid == 0) t0 = MPI_Wtime();

        // Bucle de vueltas al anillo
        for (v = 0; v < nvueltas; v++) {
            if (pid == 0) {
                // El maestro inicia: Envía -> Recibe (de la vuelta completa)
                MPI_Send(vector, tam, MPI_INT, siguiente, 0, MPI_COMM_WORLD);
                MPI_Recv(vector, tam, MPI_INT, anterior, 0, MPI_COMM_WORLD, &info);
            } else {
                // Los esclavos esperan: Reciben -> Envían
                MPI_Recv(vector, tam, MPI_INT, anterior, 0, MPI_COMM_WORLD, &info);
                MPI_Send(vector, tam, MPI_INT, siguiente, 0, MPI_COMM_WORLD);
            }
        }

        // Resultados
        if (pid == 0) {
            t1 = MPI_Wtime();
            double tiempo_total = t1 - t0;
            // Bytes totales = tamaño * sizeof(int) * num_procesos * vueltas
            double bytes_transferidos = (double)tam * sizeof(int) * npr * nvueltas;
            double ancho_banda = (bytes_transferidos / 1e6) / tiempo_total; // MB/s

            printf("Tam: 2^%d (%d ints) | Tiempo: %.4f s | BW: %.2f MB/s\n",
                   k, tam, tiempo_total, ancho_banda);
        }

        free(vector);
    }

    MPI_Finalize();
    return 0;
}