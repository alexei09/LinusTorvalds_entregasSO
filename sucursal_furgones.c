#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CLIENTES 100
#define CLIENTES_POR_FURGON 20

//Definir los semaforos y variables de control
sem_t clientes_en_sucursal;
sem_t operacion_en_curso;
pthread_mutex_t mutex;

int clientes=0;

void* llegada_cliente(void* arg){
	while(1){
		usleep(rand()% 3000000);
		pthread_mutex_lock(&mutex);
		clientes++;
		printf("Cliente entro a la sucursal. Clientes en la sucursal: %d\n", clientes);
		sem_post(&clientes_en_sucursal);
		if(clientes % CLIENTES_POR_FURGON == 0 && clientes <= MAX_CLIENTES){
			printf("\n El furgon blindado esta llegando!! La sucursal se esta vaciando...\n\n");
			sem_wait(&operacion_en_curso);
			while(clientes>0){
			    clientes--;
				printf("Cliente salio de la sucursal. Clientes en la sucursal: %d\n",clientes);
				usleep(500000);
			}
			sem_post(&operacion_en_curso);
			printf("\nEl furgon blindado puede realizar la operacion ahora. \n");
		}
		pthread_mutex_unlock(&mutex);
	}
}

void* operacion_furgon(void* arg){
    while(1){
        sem_wait(&clientes_en_sucursal);  // Esperar por clientes, fuera del mutex

        pthread_mutex_lock(&mutex);  // Proteger acceso a 'clientes'
        if(clientes % CLIENTES_POR_FURGON == 0 && clientes <= MAX_CLIENTES){
            sem_wait(&operacion_en_curso);

            printf("Furgon blindado realizando la operacion de carga/descarga de dinero...\n");
            usleep(rand()%2000000);

            printf("Furgon blindado completo la operacion.\n");
            sem_post(&operacion_en_curso);
        }
        pthread_mutex_unlock(&mutex);  // Liberar el mutex una vez que ya no necesitas acceder a 'clientes'
    }
}


int main() {
	pthread_t hilo_clientes, hilo_furgon;
	
	sem_init(&clientes_en_sucursal, 0, 0);
	sem_init(&operacion_en_curso, 0, 1);
	pthread_mutex_init(&mutex, NULL);
	
	pthread_create(&hilo_clientes, NULL, llegada_cliente, NULL);
	pthread_create(&hilo_furgon, NULL, operacion_furgon, NULL);
	
	pthread_join( hilo_clientes, NULL);
	pthread_join(hilo_furgon, NULL);
	
	sem_destroy(&clientes_en_sucursal);
	sem_destroy(&operacion_en_curso);
	pthread_mutex_destroy(&mutex);
	
	return 0;
	} 