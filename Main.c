#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_VEHICLES 10
#define MAX_WAIT_TIME 5

// Estructura para representar el puente
typedef struct {
    pthread_mutex_t mutex;
    int is_busy;       // 1 si el puente está ocupado, 0 si está libre
    char direction;    // 'E' para Este-Oeste, 'W' para Oeste-Este
    int num_crossings; // Número de vehículos que han cruzado el puente
} Bridge;

Bridge bridge;

// Estructura para representar un vehículo
typedef struct {
    char type;  // Tipo de vehículo ('E' para Este-Oeste, 'W' para Oeste-Este)
    int speed;  // Velocidad del vehículo (segundos que tarda en cruzar el puente)
} Vehicle;

// Función para que un vehículo cruce el puente
void* cross_bridge(void* arg) {
    Vehicle* vehicle = (Vehicle*) arg;

    pthread_mutex_lock(&bridge.mutex);

    // Esperar si el puente está ocupado o si el vehículo no va en la dirección correcta
    while (bridge.is_busy && (bridge.direction != vehicle->type || bridge.direction != vehicle->type)) {
        pthread_mutex_unlock(&bridge.mutex);
        sleep(1); // Esperar un segundo antes de volver a intentar
        pthread_mutex_lock(&bridge.mutex);
    }

    // Cruzar el puente (entrar)
    bridge.is_busy = 1;
    bridge.direction = vehicle->type;
    bridge.num_crossings++;

    printf("Vehiculo tipo %c ha entrado en el puente en direccion %c.\n", vehicle->type, (vehicle->type == 'E') ? 'E' : 'W');

    pthread_mutex_unlock(&bridge.mutex);

    // Simular tiempo de cruce
    sleep(vehicle->speed);

    pthread_mutex_lock(&bridge.mutex);
    bridge.is_busy = 0;
    printf("Vehiculo tipo %c ha salido del puente en direccion %c.\n", vehicle->type, (vehicle->type == 'E') ? 'E' : 'W');
    pthread_mutex_unlock(&bridge.mutex);

    free(vehicle); // Liberar memoria asignada para el vehículo

    return NULL;
}

int main() {
    pthread_t vehicles[MAX_VEHICLES];
    char vehicle_types[MAX_VEHICLES] = {'E', 'E', 'W', 'W', 'E', 'W', 'E', 'W', 'E', 'W'};

    // Inicializar el puente
    bridge.is_busy = 0;
    bridge.direction = 'E';
    bridge.num_crossings = 0;
    pthread_mutex_init(&bridge.mutex, NULL);

    // Crear threads para representar la llegada de vehículos
    for (int i = 0; i < MAX_VEHICLES; i++) {
        Vehicle* vehicle = (Vehicle*) malloc(sizeof(Vehicle));
        vehicle->type = vehicle_types[i];
        vehicle->speed = rand() % 10 + 1; // Velocidad aleatoria entre 1 y 3 segundos

        pthread_create(&vehicles[i], NULL, cross_bridge, vehicle);

        // Esperar un tiempo aleatorio antes de crear el próximo vehículo
        sleep(rand() % MAX_WAIT_TIME + 1);
    }

    // Esperar a que todos los vehículos terminen de cruzar
    for (int i = 0; i < MAX_VEHICLES; i++) {
        pthread_join(vehicles[i], NULL);
    }

    // Imprimir estadísticas finales
    printf("Todos los vehiculos han cruzado el puente. Total de cruces: %d.\n", bridge.num_crossings);

    // Liberar recursos
    pthread_mutex_destroy(&bridge.mutex);
	
	printf("Presiona Enter para salir...");
    getchar();  // Espera a que el usuario presione Enter

    return 0;
}