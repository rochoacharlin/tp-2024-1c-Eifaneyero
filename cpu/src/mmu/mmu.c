#include "mmu.h"

int tamanio_pagina = 4096; // Ejemplo de tamaño de página

int calcular_direccion_fisica(uint32_t PID, int pagina, int desplazamiento)
{
    // Asumimos que hay una función llamada buscar_frame que busca el frame en la TLB o memoria
    // int frame = buscar_frame(PID, pagina); // Esta función debe ser implementada
    // return (tamanio_pagina * frame) + desplazamiento;
    return 0;
}
