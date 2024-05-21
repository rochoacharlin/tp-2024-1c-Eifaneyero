#ifndef UTILS_ESTRUCTURAS_COMPARTIDAS_H_
#define UTILS_ESTRUCTURAS_COMPARTIDAS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/collections/list.h>

// REGISTROS DE CPU,  CPU-KERNEL
t_dictionary *crear_registros_cpu();
void destruir_registros_cpu(t_dictionary *registros_cpu);

// CONTEXTO DE EJECUCION, KERNEL<->CPU
typedef struct
{
    int PID;
    t_dictionary registros_cpu;
    int rafaga_CPU_ejecutada;
} t_contexto_ejecucion;

// PROVISORIO solicitud_de_instruccion
typedef struct
{
    uint32_t desplazamiento;
} t_solicitud_de_instruccion;

t_solicitud_de_instruccion *crear_solicitud_de_instruccion();
void destruir_solicitud_de_instruccion(t_solicitud_de_instruccion *t_solicitud_de_instruccion);

#endif