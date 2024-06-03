#ifndef CONTEXTO_EJECUCION_H
#define CONTEXTO_EJECUCION_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <utils/comunicacion/comunicacion.h>

typedef enum
{
    DESALOJO_IO_GEN_SLEEP,
    DESALOJO_WAIT,
    DESALOJO_SIGNAL,
    DESALOJO_EXIT
} motivos_desalojo;

typedef struct
{
    uint32_t PID;
    t_dictionary *registros_cpu;
} t_contexto;

// Se inicia contexto de ejecucion
t_contexto *iniciar_contexto(void);

// Destruir contexto de ejecución
void destruir_contexto(t_contexto *contexto);

// Agregado de valores a paquete
void agregar_registros_cpu_a_paquete(t_paquete *paquete, t_dictionary *registros_cpu);

// Agrego PID, registros
void agregar_contexto_a_paquete(t_contexto *contexto, t_paquete *paquete);

// Serializa y envía contexto a través del socket.
void enviar_contexto(int socket, t_contexto *contexto);

// ---------- FUNCIONES PARA RECIBO DE CONTEXTO ---------- //

// Variante fea
t_contexto *recibir_contexto(int socket);

#endif