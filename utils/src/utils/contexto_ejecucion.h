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
} t_contexto;

// Se inicia contexto de ejecucion
t_contexto *iniciar_contexto();

// Destruir contexto de ejecución
void destruir_contexto(t_contexto *contexto);

// Agregado de valores a paquete
void agregar_registros_cpu_a_paquete(t_paquete *paquete, t_dictionary *registros_cpu);

// Agrego PID, registros y rafagasDeCpu
void agregar_contexto_a_paquete(t_contexto *contexto, t_paquete *paquete);

// Serializa y envía contexto a través del socket.
void enviar_contexto(int socket, t_contexto *contexto);

// ---------- FUNCIONES PARA RECIBO DE CONTEXTO ---------- //

// Variante fea
t_contexto *recibir_contexto(int socket);

#endif