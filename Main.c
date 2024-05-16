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
    int traffic_light;  // Variable para el semáforo
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
    
    // Esperar según el semáforo (traffic_light)
    while (bridge.traffic_light != (type == 'E')) {
        printf("Semáforo en rojo para vehículo tipo '%c'. Esperando...\n", type);
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

// Función para cambiar el semáforo cada 5 segundos
void traffic_light_control() {
    while (1) {
        // Cambiar el semáforo cada 5 segundos
        bridge.traffic_light = 1 - bridge.traffic_light;  // Alternar entre 0 y 1
        printf("Cambio de semaforo: Ahora permitiendo el paso desde el %s.\n",
               (bridge.traffic_light == 1) ? "Este" : "Oeste");
        sleep(5);  // Esperar 5 segundos antes del próximo cambio
    }
}

int main() {
    //char vehicle_types[MAX_VEHICLES] = {'W','E', 'W', 'E', 'W', 'E', 'W', 'E', 'W', 'E'};
    char vehicle_types[MAX_VEHICLES] = {'W','W', 'E', 'E', 'W', 'W', 'E', 'E', 'W', 'W'};
	int vehicle_speeds[MAX_VEHICLES];

    bridge.is_busy = 0;
    bridge.direction = 'E';
    bridge.num_crossings = 0;
    bridge.traffic_light = 1;  // Inicialmente permitir cruces desde el Este (E)
    omp_init_lock(&bridge.lock);

    #pragma omp parallel for
    for (int i = 0; i < MAX_VEHICLES; i++) {
        vehicle_speeds[i] = rand() % 5 + 1; // Velocidad aleatoria entre 1 y 3 segundos
    }

    printf("Simulacion de cruce de vehiculos por un puente:\n");
    
    // Crear hilo para controlar el semáforo
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            traffic_light_control();  // Función que cambia el semáforo
        }

        #pragma omp section
        {
            // Lógica para cruzar el puente con los vehículos
            for (int i = 0; i < MAX_VEHICLES; i++) {
                // Cambiar el semáforo según el tipo de vehículo
                bridge.traffic_light = (vehicle_types[i] == 'E') ? 1 : 0;
                cross_bridge(vehicle_types[i], vehicle_speeds[i]);
                
                usleep((rand() % MAX_WAIT_TIME + 1) * 1000000);
            }
        }
    }
    
    printf("Todos los vehículos han cruzado el puente. Total de cruces: %d.\n", bridge.num_crossings);
    
    omp_destroy_lock(&bridge.lock);
    
    printf("Presiona Enter para salir...");
    getchar();  // Espera a que el usuario presione Enter
    
    return 0;
}
