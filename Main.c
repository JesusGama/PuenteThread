#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

#define MAX_VEHICLES 10
#define MAX_WAIT_TIME 5

// Estructura para representar el puente
typedef struct {
    int is_busy;       // 1 si el puente está ocupado, 0 si está libre
    char direction;    // 'E' para Este-Oeste, 'W' para Oeste-Este
    int num_crossings; // Número de vehículos que han cruzado el puente
    omp_lock_t lock;   // Lock para la sección crítica
} Bridge;

Bridge bridge;

// Función para que un vehículo cruce el puente
void cross_bridge(char type, int speed) {
    // Esperar si el puente está ocupado o si el vehículo no va en la dirección correcta
    omp_set_lock(&bridge.lock);

    printf("Vehiculo tipo '%c' intenta cruzar el puente.\n", type);
    
    while (bridge.is_busy && (bridge.direction != type)) {
        printf("Puente ocupado en direccion '%c'. Vehiculo tipo '%c' espera...\n", bridge.direction, type);
        omp_unset_lock(&bridge.lock);
        sleep(1); // Esperar un segundo antes de volver a intentar
        omp_set_lock(&bridge.lock);
    }

    // Cruzar el puente (entrar)
    bridge.is_busy = 1;
    bridge.direction = type;
    bridge.num_crossings++;

    printf("Vehiculo tipo %c ha entrado en el puente.\n", type);

    omp_unset_lock(&bridge.lock);

    // Simular tiempo de cruce
    sleep(speed);

    omp_set_lock(&bridge.lock);
    bridge.is_busy = 0;
    printf("Vehiculo tipo %c ha salido del puente.\n", type);
    omp_unset_lock(&bridge.lock);
}

int main() {
    char vehicle_types[MAX_VEHICLES] = {'E', 'E', 'W', 'W', 'E', 'W', 'E', 'W', 'E', 'W'};

    // Inicializar el puente
    bridge.is_busy = 0;
    bridge.direction = 'E';
    bridge.num_crossings = 0;
    omp_init_lock(&bridge.lock);

    printf("Simulacion de cruce de vehiculos por un puente:\n");

    // Utilizar OpenMP para paralelizar el cruce de vehículos
    #pragma omp parallel for
    for (int i = 0; i < MAX_VEHICLES; i++) {
        char type = vehicle_types[i];
        int speed = rand() % 10 + 1; // Velocidad aleatoria entre 1 y 10 segundos
        cross_bridge(type, speed);
        // Esperar un tiempo aleatorio antes de crear el próximo vehículo
        sleep(rand() % MAX_WAIT_TIME + 1);
    }

    // Imprimir estadísticas finales
    printf("Todos los vehiculos han cruzado el puente. Total de cruces: %d.\n", bridge.num_crossings);

    // Liberar recursos
    omp_destroy_lock(&bridge.lock);

    printf("Presiona Enter para salir...");
    getchar();  // Espera a que el usuario presione Enter

    return 0;
}
