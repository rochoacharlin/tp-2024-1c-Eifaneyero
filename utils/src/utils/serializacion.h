#ifndef SERIALIZACION_H
#define SERIALIZACION_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h> // library for ssize_t data type
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <string.h>
#include <assert.h> // testing

// ------------------------ CLIENTE ------------------------ //

typedef struct
{
    int size;
    void *stream;
} t_buffer;

typedef enum
{
    MENSAJE,
    PAQUETE,
    CONTEXTO_EJECUCION,
    IO_GEN_SLEEP,
    OPERACION_INVALIDA,
    OK
    /*,
    TABLADESEGMENTOS,
    READ,
    WRITE,
    NEWPCB,
    ENDPCB,
    CREATE_SEGMENT_OP,
    DELETE_SEGMENT_OP,
    TERMINAR_KERNEL,
    SUCCESS,
    OUTOFMEMORY,
    COMPACTACION,
    FOPEN,
    FCREATE,
    FTRUNCATE,
    FREAD,
    FWRITE */

} op_code;

typedef struct
{
    op_code codigo_operacion;
    t_buffer *buffer;

} t_paquete;

void enviar_mensaje(char *mensaje, int socket_cliente);
t_paquete *crear_paquete(op_code codigo_operacion);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void eliminar_paquete(t_paquete *paquete);
void enviar_cod_op(op_code codigo_de_operacion, int socket);

/* Funciones Privadas (?) */

void crear_buffer(t_paquete *paquete);
void *serializar_paquete(t_paquete *paquete, int bytes);
void agregar_a_paquete_uint32(t_paquete *paquete, uint32_t data);
void agregar_a_paquete_uint8(t_paquete *paquete, uint8_t data);

// ------------------------ SERVIDOR ------------------------ //

int recibir_operacion(int socketCliente);
void *recibir_buffer(int socket, int *size);
t_list *recibir_paquete(int socket_cliente);
char *recibir_mensaje(int socket_cliente);

#endif /* SERIALIZACION_H */