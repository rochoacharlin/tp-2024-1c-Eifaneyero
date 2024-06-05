#ifndef CPU_MMU_H
#define CPU_MMU_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils/funcionalidades_basicas.h>
#include <commons/collections/list.h>

#include "mmu.c"

extern int tamanio_pagina;

int calcular_direccion_fisica(uint32_t PID, int pagina, int desplazamiento);

#endif