#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

sem_t mostradores_comida;   // Para controlar los 5 mostradores de comida
sem_t abridores;            // Para controlar los 60 abridores
sem_t mostradores_postre;   // Para controlar los 5 mostradores de postre

// Función que simula a un soldado
void* soldado(void* arg) {
    int id = ((int)arg);
    
    // El soldado entra al comedor
    printf("Soldado %d: Entré al comedor.\n", id);
    
    // El soldado intenta tomar comida de un mostrador
    sem_wait(&mostradores_comida); // Espera a que haya un mostrador libre
    printf("Soldado %d: Estoy tomando comida de un mostrador.\n", id);
    sleep(1); // Simula el tiempo en el mostrador de comida
    sem_post(&mostradores_comida); // Libera el mostrador de comida
    printf("Soldado %d: Terminé de tomar comida.\n", id);
    
    // El soldado decide si toma agua o refresco
    int refresco = rand() % 2; // 50% de probabilidad de elegir refresco
    if (refresco) {
        sem_wait(&abridores); // Espera un abridor si eligió refresco
        printf("Soldado %d: Tomé un refresco y usé un abridor.\n", id);
        sleep(1); // Simula el uso del abridor
        sem_post(&abridores); // Libera el abridor
    } else {
        printf("Soldado %d: Tomé agua, no necesito abridor.\n", id);
    }
    
    // El soldado decide si quiere postre
    int quiere_postre = rand() % 2; // 50% de probabilidad de querer postre
    if (quiere_postre) {
        sem_wait(&mostradores_postre); // Espera a que haya un mostrador de postre libre
        printf("Soldado %d: Estoy tomando postre.\n", id);
        sleep(1); // Simula el tiempo en el mostrador de postre
        sem_post(&mostradores_postre); // Libera el mostrador de postre
        printf("Soldado %d: Terminé de tomar postre.\n", id);
    } else {
        printf("Soldado %d: No quiero postre.\n", id);
    }
    
    // El soldado sale del comedor
    printf("Soldado %d: Terminé de comer y salí del comedor.\n", id);
    
    return NULL;
}

int main() {
    int num_soldados = 1000;
    pthread_t soldados[num_soldados];
    int id_soldados[num_soldados];
    
    // Inicializamos los semáforos
    sem_init(&mostradores_comida, 0, 5);  // Hay 5 mostradores de comida
    sem_init(&abridores, 0, 60);          // Hay 60 abridores disponibles
    sem_init(&mostradores_postre, 0, 5);  // Hay 5 mostradores de postre

    // Creamos los hilos para simular los soldados
    for (int i = 0; i < num_soldados; i++) {
        id_soldados[i] = i + 1;
        pthread_create(&soldados[i], NULL, soldado, &id_soldados[i]);
    }

    // Esperamos a que todos los soldados terminen de comer
    for (int i = 0; i < num_soldados; i++) {
        pthread_join(soldados[i], NULL);
    }

    // Destruimos los semáforos
    sem_destroy(&mostradores_comida);
    sem_destroy(&abridores);
    sem_destroy(&mostradores_postre);
    
    return 0;
}
