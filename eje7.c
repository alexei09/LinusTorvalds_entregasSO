#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Definimos constantes para los tiempos de acceso
#define TLB_ACCESS_TIME 8  // Tiempo de acceso a TLB en ns
#define MEM_ACCESS_TIME 70 // Tiempo de acceso a memoria principal en ns

// Tamaño de la página (2^13 = 8192 bytes = 8KB)
#define PAGE_SIZE 8192

// Estructura para la dirección virtual
typedef struct {
    unsigned int offset : 13; // Offset dentro de la página (13 bits)
    unsigned int lvl3_idx : 7; // Índice de la tabla de nivel 3 (7 bits)
    unsigned int lvl2_idx : 8; // Índice de la tabla de nivel 2 (8 bits)
    unsigned int lvl1_idx : 8; // Índice de la tabla de nivel 1 (8 bits)
} virtual_address_t;

// Función para simular el acceso al TLB
bool access_tlb() {
    // Asumimos que la tasa de aciertos del TLB es del 90%
    return rand() % 100 < 90;
}

// Simula el acceso a memoria usando tablas de páginas de tres niveles
int access_memory(virtual_address_t v_addr) {
    int access_time = 0;

    if (access_tlb()) {
        // Acierto en TLB
        access_time += TLB_ACCESS_TIME;
        access_time += MEM_ACCESS_TIME; // Acceso a la memoria principal
    } else {
        // Falla en TLB, necesitamos acceder a las tablas de páginas
        access_time += TLB_ACCESS_TIME; // Intento fallido en TLB

        // Acceso a tablas de páginas (3 niveles)
        access_time += 3 * MEM_ACCESS_TIME; // Acceso a las tablas de nivel 1, 2 y 3
        access_time += MEM_ACCESS_TIME; // Finalmente, acceso a la página en memoria
    }

    return access_time;
}

int main() {
    srand(42); // Semilla para los aciertos de TLB

    // Creamos una dirección virtual de ejemplo
    virtual_address_t v_addr;
    v_addr.offset = 0x1234;
    v_addr.lvl3_idx = 0x12;
    v_addr.lvl2_idx = 0x34;
    v_addr.lvl1_idx = 0x56;

    // Calculamos el tiempo de acceso para esta dirección
    int total_access_time = access_memory(v_addr);

    // Imprimimos el resultado
    printf("Tiempo total de acceso a memoria: %d ns\n", total_access_time);

    return 0;
}
