#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

double pi_secuencial(size_t numDarts) {
    size_t dartsInCircle = 0;
    double x, y;

    for (double i = 0; i < numDarts; i++) {
        // Generar coordenadas aleatorias dentro del cuadrado
        x = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
        y = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
        // Verificar si el dardo cae dentro de la circunferencia
        if (x * x + y * y <= 1.0) {
            dartsInCircle++;
        }
    }

    // Calcular la aproximación de pi
    return 4.0 * (double)dartsInCircle / (double)numDarts;
}

double pi_paralelo(int numThreads, size_t numDarts) {
    size_t dartsInCircle = 0;
    double x, y;

    #pragma omp parallel num_threads(numThreads) reduction(+:dartsInCircle)
{
        // Generación de una semilla única para cada hilo
        unsigned int seed = (unsigned int)time(NULL) + omp_get_thread_num();
        srand(seed);
        
        #pragma omp for schedule(static, numDarts / numThreads)
        for (size_t i = 0; i < numDarts; i++) {
            // Generar coordenadas aleatorias dentro del cuadrado
            x = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
            y = ((double)rand() / (double)RAND_MAX) * 2.0 - 1.0;
            // Verificar si el dardo cae dentro de la circunferencia
            if (x * x + y * y <= 1.0) {
                dartsInCircle++;
            }
        }
    }

    // Calcular la aproximación de pi
    return 4.0 * (double)dartsInCircle / (double)numDarts;
}

int main() {
    int numThreads; // Número de hilos a utilizar
    size_t numDarts; // Número de dardos a utilizar en la Aproximacion
    double approxPiSequential, approxPiParallel;
    double startTime, endTime, totalStartTime, totalEndTime, totalExecutionTime;
    double sequentialTime, parallelTime;
    double promedioSecuencial = 0.0, promedioParalelo = 0.0, promedioSpeedup = 0.0, promedioEficiencia = 0.0;
    int maxThreads = 20; // Número máximo de hilos a utilizar

    FILE *file = fopen("output.txt", "w"); // Abrir archivo en modo escritura

    printf("Ingrese el numero de dardos a utilizar en la aproximacion: ");
    scanf("%zd", &numDarts);
    fprintf(file, "Numero de dardos a utilizar en la aproximacion: %zd\n", numDarts);

    totalStartTime = omp_get_wtime(); // Iniciar conteo de tiempo total
    for (numThreads = 1; numThreads <= maxThreads; numThreads++) { // Prueba con 1 hilo hasta el máximo de hilos permitidos
        for (int i = 1; i <= 10; i++) { // Se ejecuta 10 veces para obtener un promedio

            if (numThreads == 1) { // Se ejecuta una vez para no repetir el tiempo de ejecución secuencial
                // Aproximacion secuencial
                startTime = omp_get_wtime();
                approxPiSequential = pi_secuencial(numDarts);
                endTime = omp_get_wtime();
                sequentialTime = endTime - startTime;
                promedioSecuencial += sequentialTime;

                fprintf(file, "Aproximacion secuencial de pi[%d]: %.16lf\n", i, approxPiSequential);
                fprintf(file, "Tiempo de ejecucion secuencial[%d]: %lf segundos\n\n", i, sequentialTime);
            }

            // Aproximacion paralela
            startTime = omp_get_wtime();
            approxPiParallel = pi_paralelo(numThreads, numDarts);
            endTime = omp_get_wtime();
            parallelTime = endTime - startTime;

            fprintf(file, "Hilo[%d] Aproximacion paralela de pi[%d]: %.16lf\n", numThreads, i, approxPiParallel);
            fprintf(file, "Hilo[%d] Tiempo de ejecucion paralelo[%d]: %lf segundos\n", numThreads, i, parallelTime);

            // Métricas
            double speedup = sequentialTime / parallelTime;
            double efficiency = speedup / numThreads;

            fprintf(file, "Hilo[%d] Speedup[%d]: %lf\n", numThreads, i, speedup);
            fprintf(file, "Hilo[%d] Eficiencia[%d]: %lf\n\n", numThreads, i, efficiency);

            // Promedios
            promedioParalelo += parallelTime;
            promedioSpeedup += speedup;
            promedioEficiencia += efficiency;
        }
        if (numThreads == 1) {
            promedioSecuencial /= 10;
            fprintf(file, "Promedio Tiempo Secuencial: %lf\n", promedioSecuencial);
        }
        promedioParalelo /= 10;
        promedioSpeedup /= 10;
        promedioEficiencia /= 10;

        fprintf(file, "Hilo[%d] Promedio Tiempo Paralelo: %lf\n", numThreads, promedioParalelo);
        fprintf(file, "Hilo[%d] Promedio Speedup: %lf\n", numThreads, promedioSpeedup);
        fprintf(file, "Hilo[%d] Promedio Eficiencia: %lf\n\n\n", numThreads, promedioEficiencia);
    }
    totalEndTime = omp_get_wtime(); // Finalizar conteo de tiempo total
    totalExecutionTime = totalEndTime - totalStartTime;
    fprintf(file, "Tiempo de ejecucion total: %lf segundos\n", totalExecutionTime);
    fprintf(file, "Tiempo de ejecucion secuencial: %lf segundos\n", promedioSecuencial * 10);
    fprintf(file, "Tiempo de ejecucion paralelo: %lf segundos\n", totalExecutionTime - (promedioSecuencial * 10));
    fclose(file); // Cerrar archivo

    return 0;
}
