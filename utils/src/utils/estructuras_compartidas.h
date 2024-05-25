#ifndef UTILS_ESTRUCTURAS_COMPARTIDAS_H_
#define UTILS_ESTRUCTURAS_COMPARTIDAS_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include "serializacion.h"

// REGISTROS DE CPU,  CPU-KERNEL
t_dictionary *crear_registros_cpu();
uint32_t obtener_valor_registro(t_dictionary *registros_cpu, char *nombre_registro);
void destruir_registros_cpu(t_dictionary *registros_cpu);

// Copia diccionario de registros de CPU de un dict a otro
t_dictionary *copiar_registros_cpu(t_dictionary *a_copiar);

// PROVISORIO solicitud_de_instruccion
typedef struct
{
    uint32_t desplazamiento;
} t_solicitud_de_instruccion;

t_solicitud_de_instruccion *crear_solicitud_de_instruccion();
void destruir_solicitud_de_instruccion(t_solicitud_de_instruccion *t_solicitud_de_instruccion);
void serializar_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud, void *buffer);
void generar_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud, t_list *valores);
int tamanio_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud);

typedef struct
{
    char *instruccion;
} t_instruccion;

t_instruccion *crear_instruccion();
void destruir_instruccion(t_instruccion *instruccion);
void serializar_instruccion(t_instruccion *instruccion, void *buffer);
void generar_instruccion(t_instruccion *instruccion, t_list *valores);
int tamanio_instruccion(t_instruccion *instruccion);

#endif