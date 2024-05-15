#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <unistd.h>

#define MAX_VEHICLES 10
#define MAX_WAIT_TIME 5

typedef struct {
    omp_lock_t lock;
    int is_busy;
    char direction;
    int num_crossings;
} Bridge;

Bridge bridge;

void cross_bridge(char type, int speed) {
    omp_set_lock(&bridge.lock);
    
    printf("Vehiculo tipo '%c' intenta cruzar el puente en direccion '%c'.\n", type, type);
    
    while (bridge.is_busy && (bridge.direction != type)) {
        printf("Puente ocupado en direccion '%c'. Vehiculo tipo '%c' espera...\n", bridge.direction, type);
        omp_unset_lock(&bridge.lock);
        sleep(1);
        omp_set_lock(&bridge.lock);
    }
    
    bridge.is_busy = 1;
    bridge.direction = type;
    bridge.num_crossings++;
    
    printf("Vehiculo tipo '%c' cruza el puente en direccion '%c'.\n", type, type);
    
    omp_unset_lock(&bridge.lock);
    
    sleep(speed);
    
    omp_set_lock(&bridge.lock);
    bridge.is_busy = 0;
    printf("Vehiculo tipo '%c' ha salido del puente en direccion '%c'.\n", type, type);
    omp_unset_lock(&bridge.lock);
}

int main() {
    //char vehicle_types[MAX_VEHICLES] = {'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E', 'W'};
	char vehicle_types[MAX_VEHICLES] = {'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'W'};
	//char vehicle_types[MAX_VEHICLES] = {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'};
    int vehicle_speeds[MAX_VEHICLES];

    bridge.is_busy = 0;
    bridge.direction = 'E';
    bridge.num_crossings = 0;
    omp_init_lock(&bridge.lock);

    #pragma omp parallel for
    for (int i = 0; i < MAX_VEHICLES; i++) {
        vehicle_speeds[i] = rand() % 3 + 1; // Velocidad aleatoria entre 1 y 3 segundos
    }

    printf("Simulacion de cruce de vehiculos por un puente:\n");
    
    #pragma omp parallel for
    for (int i = 0; i < MAX_VEHICLES; i++) {
        cross_bridge(vehicle_types[i], vehicle_speeds[i]);
        
        usleep((rand() % MAX_WAIT_TIME + 1) * 1000000);
    }
    
    printf("Todos los vehículos han cruzado el puente. Total de cruces: %d.\n", bridge.num_crossings);
    
    omp_destroy_lock(&bridge.lock);
	
	printf("Presiona Enter para salir...");
    getchar();  // Espera a que el usuario presione Enter
    
    return 0;
}
