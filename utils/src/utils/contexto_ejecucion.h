#ifndef CONTEXTO_EJECUCION_H
#define CONTEXTO_EJECUCION_H

#include "stdint.h"
// #include "stdlib.h"
#include "utils/serializacion.h"

typedef struct
{
    uint8_t PID;
    t_dictionary *registros_cpu;
    uint8_t rafaga_cpu_ejecutada;
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
// Hay otra función para recibir OpCode
void recibir_contexto_actualizado(int socket);
// Los registros tienen una deserialización particular porque no guardan tamaño before each value
// TODO: void deserializar_registros(void *buffer, int *desplazamiento);

#endif