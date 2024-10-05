#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define CAPACIDAD_CINTA 5  // Capacidad de la cinta transportadora
#define TOTAL_PIEZAS 20    // Total de piezas que produce la máquina

sem_t mutex;              // Para proteger el acceso a la cinta transportadora
sem_t piezas_robot;       // Controla las piezas disponibles para el robot
sem_t piezas_operario;    // Controla si hay al menos 3 piezas disponibles para el operario
sem_t espacio_disponible; // Controla el espacio disponible en la cinta

int cinta_transportadora = 0;  // Contador de piezas en la cinta

// Función de la máquina de inyección
void* maquina_inyectar(void* arg) {
    for (int i = 0; i < TOTAL_PIEZAS; i++) {
        sem_wait(&espacio_disponible);  // Espera a que haya espacio en la cinta
        sem_wait(&mutex);               // Acceso exclusivo a la cinta

        cinta_transportadora++;  // Añade una pieza a la cinta
        printf("Máquina: pieza producida. Piezas en la cinta: %d\n", cinta_transportadora);

        sem_post(&mutex);        // Libera el acceso a la cinta
        sem_post(&piezas_robot); // Indica que hay una pieza para el robot

        // Si hay al menos 3 piezas, el operario puede hacer control de calidad
        if (cinta_transportadora > 3) {
            sem_post(&piezas_operario); // Informa que el operario puede actuar
        }
        

        sleep(1);  // Simula el tiempo de producción
    }
    //return NULL;
}

// Función del robot que recoge piezas de la cinta
void* robot_recolector(void* arg) {
    sleep (2);
    for (int i = 0; i < TOTAL_PIEZAS; i++) {
        sem_wait(&piezas_robot);  // Espera a que haya una pieza en la cinta para el robot
        sem_wait(&mutex);         // Acceso exclusivo a la cinta
    if(cinta_transportadora > 0){
        cinta_transportadora--;  // El robot recoge una pieza de la cinta
        printf("Robot: pieza recogida. Piezas en la cinta: %d\n", cinta_transportadora);

        sem_post(&mutex);        // Libera el acceso a la cinta
        sem_post(&espacio_disponible);  // Libera un espacio en la cinta

        sleep(4);  // Simula el tiempo de recolección del robot
    }
    }
  //  return NULL;
}

// Función del operario que realiza el control de calidad
void* operario_control_calidad(void* arg) {
    while (1) {
        sem_wait(&piezas_operario);  // Espera hasta que haya al menos 3 piezas disponibles

        sem_wait(&mutex);  // Acceso exclusivo a la cinta

        if (cinta_transportadora > 3) {
            cinta_transportadora -= 3;  // El operario recoge 3 piezas
            printf("Operario: control de calidad realizado. Piezas restantes en la cinta: %d\n", cinta_transportadora);

            sem_post(&espacio_disponible);  // Libera 3 espacios en la cinta
            sem_post(&espacio_disponible);
            sem_post(&espacio_disponible);
        }

        sem_post(&mutex);  // Libera el acceso a la cinta

        sleep(5);  // El operario espera un tiempo antes de intentar nuevamente
    }
    //return NULL;
}

int main() {
    pthread_t hilo_maquina, hilo_robot, hilo_operario;

    // Inicialización de semáforos
    sem_init(&mutex, 0, 1);
    sem_init(&piezas_robot, 0, 0);        // Para piezas disponibles para el robot
    sem_init(&piezas_operario, 0, 0);     // Para piezas disponibles para el operario
    sem_init(&espacio_disponible, 0, CAPACIDAD_CINTA);  // Espacio en la cinta

    // Creación de los hilos
    pthread_create(&hilo_maquina, NULL, maquina_inyectar, NULL);
    pthread_create(&hilo_robot, NULL, robot_recolector, NULL);
    pthread_create(&hilo_operario, NULL, operario_control_calidad, NULL);

    // Espera a que los hilos terminen
    pthread_join(hilo_maquina, NULL);
    pthread_join(hilo_robot, NULL);
    // El hilo del operario se ejecuta indefinidamente (control de calidad), por lo que no lo unimos.
    
    // Destrucción de semáforos
    sem_destroy(&mutex);
    sem_destroy(&piezas_robot);
    sem_destroy(&piezas_operario);
    sem_destroy(&espacio_disponible);
    
    return 0;
}
