#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

#define CACHE_SIZE 5
#define TIME_QUANTUM 1 // Tiempo de ejecución para Round Robin

typedef struct {
    int id;
    char data[20];
} CacheItem;

typedef struct {
    int cache[CACHE_SIZE];
    int usage[CACHE_SIZE];
} SharedMemory;

sem_t *sem; // Semáforo para coordinar el acceso concurrente
pthread_mutex_t mutex; // Mutex para asegurar que solo un hilo modifique la caché a la vez

int findLRU(int usage[]) {
    int minUsage = usage[0], lruIndex = 0;
    for (int i = 1; i < CACHE_SIZE; i++) {
        if (usage[i] < minUsage) {
            minUsage = usage[i];
            lruIndex = i;
        }
    }
    return lruIndex;
}

void *accessCache(void *arg) {
    SharedMemory *shm = (SharedMemory *)arg;
    CacheItem item = {rand() % 100, "Cache data"}; // Generación de ID aleatorio para cambiar en cada ejecución

    sem_wait(sem); // Bloqueo del semáforo
    pthread_mutex_lock(&mutex); // Bloqueo del mutex

    int found = 0;
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (shm->cache[i] == item.id) {
            found = 1;
            shm->usage[i]++; // Incremento del uso si encontramos el elemento
            break;
        }
    }

    if (!found) {
        int lruIndex = findLRU(shm->usage); // Búsqueda del elemento menos usado
        printf("Reemplazando elemento ID: %d con ID: %d\n", shm->cache[lruIndex], item.id);
        shm->cache[lruIndex] = item.id; // Reemplazo del LRU con el nuevo
        shm->usage[lruIndex] = 1; // Reinicio del uso
    }

    pthread_mutex_unlock(&mutex); // Liberación del mutex
    sem_post(sem); // Liberación del semáforo

    return NULL;
}

void initializeCache(SharedMemory *shm) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        shm->cache[i] = -1; // -1 indica que el espacio está vacío
        shm->usage[i] = 0; // Inicialización del uso en 0
    }
}

void printCache(SharedMemory *shm) {
    printf("Contenido de la cache:\n");
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (shm->cache[i] != -1) {
            printf("ID: %d, Usage: %d\n", shm->cache[i], shm->usage[i]);
        }
    }
}

void simulateRoundRobin(SharedMemory *shm) {
    pthread_t threads[5]; // Creación de hilos para Round Robin
    for (int i = 0; i < 5; i++) {
        pthread_create(&threads[i], NULL, accessCache, shm); // Creación de hilos
    }
    for (int i = 0; i < 5; i++) {
        pthread_join(threads[i], NULL); // Espera a los hilos
        sleep(TIME_QUANTUM); // Simulación del tiempo de ejecución para Round Robin
    }
}

int main() {
    srand(time(NULL)); // Semilla aleatoria basada en el tiempo actual

    sem = sem_open("/cacheSem", O_CREAT, 0644, 1); // Creación del semáforo
    pthread_mutex_init(&mutex, NULL); // Inicialización del mutex

    int fd[2];
    pipe(fd); // Creación del pipe

    SharedMemory *shm = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); // Creación de memoria compartida
    initializeCache(shm);
    
    pid_t pid = fork();
    if (pid == 0) { // Proceso hijo
        close(fd[0]); // Cierre del extremo de lectura del pipe
        simulateRoundRobin(shm); // Planificación Round Robin para hilos
        write(fd[1], shm, sizeof(SharedMemory)); // Escritura en el pipe
        close(fd[1]); // Cierre del extremo de escritura del pipe
        exit(0);
    } else { // Proceso padre
        wait(NULL); // Espera al proceso hijo (FIFO)
        close(fd[1]); // Cierre del extremo de escritura del pipe
        read(fd[0], shm, sizeof(SharedMemory)); // Lectura del pipe
        close(fd[0]); // Cierre del extremo de lectura del pipe
        printCache(shm);
    }

    sem_close(sem); // Cierre del semáforo
    sem_unlink("/cacheSem"); // Eliminación del semáforo
    pthread_mutex_destroy(&mutex); // Destrucción del mutex
    munmap(shm, sizeof(SharedMemory)); // Desmapear la memoria compartida
    return 0;
}
