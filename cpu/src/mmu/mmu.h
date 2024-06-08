#ifndef CPU_MMU_H
#define CPU_MMU_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils/funcionalidades_basicas.h>
#include <commons/collections/list.h>
#include "configuraciones.h"

extern int tamanio_pagina;

int calcular_direccion_fisica(uint32_t PID, int pagina, int desplazamiento);

typedef struct
{
    unint32_t PID;
    int pagina;
    int marco;
    int tiempo; // LRU
} t_registro_TLB;

typedef struct
{
    t_registro_TLB *TLB;
    int entradas_utilizadas;
    int tiempo_actual;
} t_TLB;

t_TLB crear_TLB();
void destruir_TLB(t_TLB *tlb);
int buscar_en_TLB(t_TLB *tlb, uint32_t PID, int pagina);
void agregar_pagina_TLB(t_TLB *tlb, uint32_t PID, int pagina, int marco);
void reemplazar_pagina_TLB(t_TLB *tlb, uint32_t PID, int pagina, int marco);
int obtener_indice_para_reemplazo(t_TLB *tlb);

#endif