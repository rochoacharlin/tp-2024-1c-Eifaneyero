#ifndef CONTEXTO_EJECUCION_H
#define CONTEXTO_EJECUCION_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "serializacion.h"

typedef struct
{
    uint32_t PID;
    t_dictionary *registros_cpu;
    uint64_t rafaga_cpu_ejecutada;
} t_contexto_ejecucion;

extern t_contexto_ejecucion *contexto_ejecucion;

// Se inicia contexto de ejecucion
void iniciar_contexto();

// Destruir contexto de ejecución
void destruir_contexto();

// Agregado de valores a paquete
void agregar_registros_cpu_a_paquete(t_paquete *paquete, t_dictionary *registros_cpu);

// Serializa y envía contexto a través del socket.
void enviar_contexto_actualizado(int socket);

// ---------- FUNCIONES PARA RECIBO DE CONTEXTO ---------- //

// Variante fea
void recibir_contexto_y_actualizar_global(int socket);

#endif