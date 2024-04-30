#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

#define MAX_VEHICLES 25
#define MAX_WAIT_TIME 5

// Estructura para representar el puente
typedef struct {
    omp_lock_t lock;
    int is_busy;       // 1 si el puente está ocupado, 0 si está libre
    char direction;    // 'E' para Este-Oeste, 'W' para Oeste-Este
    int num_crossings; // Número de vehículos que han cruzado el puente
} Bridge;

Bridge bridge;

// Función para que un vehículo cruce el puente
void cross_bridge(char type, int speed) {
    omp_set_lock(&bridge.lock);
    
    // Esperar si el puente está ocupado o si el vehículo no va en la dirección correcta
    while (bridge.is_busy && (bridge.direction != type)) {
        omp_unset_lock(&bridge.lock);
        sleep(1); // Esperar un segundo antes de volver a intentar
        omp_set_lock(&bridge.lock);
    }
    
    // Cruzar el puente (entrar)
    bridge.is_busy = 1;
    bridge.direction = type;
    bridge.num_crossings++;
    
    printf("Vehiculo cruzando el puente en direccion %c.\n", type);
    
    omp_unset_lock(&bridge.lock);
    
    // Simular tiempo de cruce
    sleep(speed);
    
    omp_set_lock(&bridge.lock);
    bridge.is_busy = 0;
    printf("Vehiculo ha salido del puente en direccion %c.\n", type);
    omp_unset_lock(&bridge.lock);
}

int main() {
    char vehicle_types[MAX_VEHICLES] = {'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'W', 'E', 'E', 'E', 'W'};
    int vehicle_speeds[MAX_VEHICLES];

    // Inicializar el puente
    bridge.is_busy = 0;
    bridge.direction = 'E';
    bridge.num_crossings = 0;
    omp_init_lock(&bridge.lock);

    // Asignar velocidades aleatorias a los vehículos
    #pragma omp parallel for
    for (int i = 0; i < MAX_VEHICLES; i++) {
        vehicle_speeds[i] = rand() % 10 + 1; // Velocidad aleatoria entre 1 y 3 segundos
    }

    // Procesar los vehículos en paralelo
    #pragma omp parallel for
    for (int i = 0; i < MAX_VEHICLES; i++) {
        cross_bridge(vehicle_types[i], vehicle_speeds[i]);
        
        // Esperar un tiempo aleatorio antes de crear el próximo vehículo
        usleep((rand() % MAX_WAIT_TIME + 1) * 1000000); // usleep trabaja en microsegundos
    }
    
    // Imprimir estadísticas finales
    printf("Todos los vehiculos han cruzado el puente. Total de cruces: %d.\n", bridge.num_crossings);
    
    // Liberar recursos
    omp_destroy_lock(&bridge.lock);
    
    return 0;
}
