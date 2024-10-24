#include <stdio.h>
#include <stdint.h>

#define PAGE_SIZE 4096   // Tamaño de página en bytes (4KB)
#define NUM_PAGES 8      // Número total de páginas

// Estructura que define una entrada en la tabla de páginas
typedef struct {
    int presencia;  // Bit de presencia (1 si está en memoria física, 0 si está en swap)
    int modificado; // Bit de modificado (no utilizado en este ejemplo, pero puede extenderse)
    int marco;      // Marco de página o bloque de swap
} Pagina;

Pagina tablaPaginas[NUM_PAGES] = {
    {1, 0, 1},   // Página 0 está en marco 1
    {0, 0, 8},   // Página 1 está en swap en bloque 8
    {1, 0, 9},   // Página 2 está en marco 9
    {1, 0, 14},  // Página 3 está en marco 14
    {1, 0, 5},   // Página 4 está en marco 5
    {0, 0, 7},   // Página 5 está en swap en bloque 7
    {1, 0, 25},  // Página 6 está en marco 25
    {0, 1, 16}   // Página 7 está en swap en bloque 16
};

// Función para traducir una dirección virtual a una dirección física
uint32_t traducirDireccionVirtual(uint32_t direccionVirtual) {
    // Extraer el número de página y el desplazamiento de la dirección virtual
    uint32_t numPagina = direccionVirtual / PAGE_SIZE;
    uint32_t desplazamiento = direccionVirtual % PAGE_SIZE;

    // Verificar si la página está presente en memoria física
    if (tablaPaginas[numPagina].presencia == 1) {
        uint32_t direccionFisica = (tablaPaginas[numPagina].marco * PAGE_SIZE) + desplazamiento;
        return direccionFisica;
    } else {
        printf("La página %u no está en memoria física, está en swap en el bloque %u.\n",
               numPagina, tablaPaginas[numPagina].marco);
        return 0xFFFFFFFF;  // Retornar un valor inválido si la página está en swap
    }
}

int main() {
    // Parte (a): Formato de la dirección virtual
    printf("Parte (a): Formato de la dirección virtual\n");
    printf("La dirección virtual tiene 32 bits, los primeros 20 bits son el número de página y los últimos 12 bits son el desplazamiento.\n\n");

    // Parte (b): Traducción de una dirección virtual a física
    uint32_t direccionVirtual = 0x00005B83;  // Dirección virtual dada
    printf("Parte (b): Traducción de la dirección virtual 0x%08X\n", direccionVirtual);
    uint32_t direccionFisica = traducirDireccionVirtual(direccionVirtual);
    if (direccionFisica != 0xFFFFFFFF) {
        printf("La dirección física correspondiente es: 0x%08X\n", direccionFisica);
    } else {
        printf("La página no está presente en memoria física.\n");
    }

    // Parte (c): Tamaño ocupado por el espacio de direcciones virtual de este programa
    printf("\nParte (c): Tamaño del espacio de direcciones virtuales ocupado\n");
    int paginasUsadas = 0;
    for (int i = 0; i < NUM_PAGES; i++) {
        if (tablaPaginas[i].presencia == 1 || tablaPaginas[i].marco != -1) {
            paginasUsadas++;
        }
    }
    printf("El tamaño ocupado por el espacio de direcciones virtuales es: %d KB\n", paginasUsadas * 4);

    return 0;
}
