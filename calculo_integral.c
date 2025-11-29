/*
 * MPI: integral_mpi.c
 * Calculo de integral mediante suma de trapecios (Regla del Trapecio)
 * Uso de MPI_Bcast y MPI_Reduce
 */

#include <mpi.h>
#include <stdio.h>
#include <math.h>

// Función matemática a integrar
double f(double x) {
    return 1.0/(sin(x)+2.0) + 1.0/(sin(x)*cos(x)+2.0);
}

// Función para el cálculo local de trapecios
double Integrar(double a_loc, int n_loc, double w) {
    double resul_loc = 0.0;
    double x;
    int i;

    // x comienza en el primer punto de este proceso
    x = a_loc;

    // Suma de las alturas (áreas de los trapecios)
    // Nota: Para simplificar, usamos la suma de Riemann izquierda/trapecio simple
    for (i = 0; i < n_loc; i++) {
        resul_loc += f(x);
        x = x + w;
    }

    return resul_loc * w;
}

int main(int argc, char** argv) {
    int pid, npr;
    double a, b, w;
    int n;

    // Variables locales
    double a_loc;
    int n_loc;
    double resul_loc, resul_total;

    // Variables de tiempo
    double t0, t1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &npr);

    // 1. Lectura de datos (Solo el proceso 0)
    if (pid == 0) {
        // Valores definidos en el ejercicio (Pag 25)
        a = 0.0;
        b = 100.0;
        n = 11200000; // Puedes cambiar esto por scanf si prefieres

        printf("Calculando integral de %.1f a %.1f con %d trapecios...\n", a, b, n);
        t0 = MPI_Wtime(); // Inicio del cronómetro
    }

    // 2. Distribución de datos (Broadcast)
    // Enviamos a, b, y n desde el proceso 0 a todos los demás
    MPI_Bcast(&a, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // 3. Cálculos previos comunes
    w = (b - a) / n;      // Ancho de cada trapecio
    n_loc = n / npr;      // Cuántos trapecios calcula cada proceso

    // Punto de inicio local:
    // Si soy el proceso 2, salto los trapecios que hicieron el 0 y el 1.
    a_loc = a + (pid * n_loc * w);

    // 4. Cálculo Local
    resul_loc = Integrar(a_loc, n_loc, w);

    // 5. Recolección de resultados (Reduce)
    // Sumamos todos los resul_loc y guardamos el resultado en resul_total en el proceso 0
    MPI_Reduce(&resul_loc, &resul_total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // 6. Impresión de resultados
    if (pid == 0) {
        t1 = MPI_Wtime();
        printf("-----------------------------------------\n");
        printf("Resultado Integral: %.12f\n", resul_total);
        printf("Tiempo de ejecución (%d proc): %1.4f ms\n", npr, (t1 - t0) * 1000);
        printf("-----------------------------------------\n");
    }

    MPI_Finalize();
    return 0;
}