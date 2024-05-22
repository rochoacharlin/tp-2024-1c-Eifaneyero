#ifndef UTILS_ESTRUCTURAS_COMPARTIDAS_H_
#define UTILS_ESTRUCTURAS_COMPARTIDAS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "utils/serializacion.h"

// REGISTROS DE CPU,  CPU-KERNEL
t_dictionary *crear_registros_cpu();
uint32_t obtener_valor_registro(t_dictionary *registros_cpu, char *nombre_registro);
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
void serializar_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud, t_paquete *paquete);
void deserializar_solicitud_de_instruccion();
int tamanio_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud);

#endif