#include <stdio.h>
#include <stdlib.h>

#define MEMORIA_TOTAL 1024 // 1 MB en KB
#define TAMANIO_BLOQUE 4 // 4 KB
#define TAMANIO_MAPA_BITS (MEMORIA_TOTAL / TAMANIO_BLOQUE)

unsigned char mapa_bits[TAMANIO_MAPA_BITS] = {0}; // Inicializamos el mapa de bits

void crear_archivo(int id_archivo, int size) {
    int bloques_necesarios = size / TAMANIO_BLOQUE;
    if (size % TAMANIO_BLOQUE != 0) {
        bloques_necesarios++; // Redondea hacia arriba si hay un residuo
    }

    // Encontrar bloques contiguos libres
    int encontrados = 0, inicio = -1;
    for (int i = 0; i < TAMANIO_MAPA_BITS; i++) {
        if (mapa_bits[i] == 0) {
            if (inicio == -1) inicio = i;
            encontrados++;
            if (encontrados == bloques_necesarios) break;
        } else {
            encontrados = 0;
            inicio = -1;
        }
    }

    if (encontrados == bloques_necesarios) {
        // Marcar bloques como ocupados
        for (int i = inicio; i < inicio + bloques_necesarios; i++) {
            mapa_bits[i] = 1;
        }
        printf("Archivo %d creado: %d KB desde el bloque %d al %d\n", id_archivo, size, inicio, inicio + bloques_necesarios - 1);
    } else {
        printf("Error: No hay suficiente memoria disponible para el archivo %d\n", id_archivo);
    }
}

void eliminar_archivo(int id_archivo) {
    // Para simplicidad, liberaremos todos los bloques ocupados en este ejemplo
    for (int i = 0; i < TAMANIO_MAPA_BITS; i++) {
        if (mapa_bits[i] == 1) {
            mapa_bits[i] = 0;
        }
    }
    printf("Archivo %d eliminado\n", id_archivo);
}

void mostrar_estado_memoria() {
    printf("Estado actual de la memoria (0: libre, 1: ocupada):\n");
    for (int i = 0; i < TAMANIO_MAPA_BITS; i++) {
        printf("%d", mapa_bits[i]);
    }
    printf("\n");
}

int main() {
    // Simulación de la gestión de archivos
    crear_archivo(1, 20); // Ejemplo: Crear un archivo de 20 KB
    mostrar_estado_memoria();
    eliminar_archivo(1); // Ejemplo: Eliminar el archivo 1
    mostrar_estado_memoria();
    
    return 0;
}
