#ifndef CICLO_DE_INSTRUCCION_H
#define CICLO_DE_INSTRUCCION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/estructuras_compartidas/contexto_ejecucion.h>
#include <utils/estructuras_compartidas/instrucciones_cpu.h>
#include <utils/estructuras_compartidas/instrucciones.h>
#include "configuraciones.h"
// #include "conexiones/conexiones.h" //Para usar conexiones globales
#include "interface_cpu.h"

// ID instruccion

typedef struct
{
    t_id id;
    char *param1;
    char *param2;
    char *param3;
    char *param4;
    char *param5;
} t_instruccion;

// Version 2.0: Posiblemente quedaba mejor
// typedef struct {
//     t_id id;
//     char* parametros[];
// }t_instruccion;

// extern pthread_mutex_t mutex_interrupt;

// -------------------- CICLO DE INSTRUCCION -------------------- //

void ciclo_de_instruccion(t_contexto *contexto);
t_instruccion *fetch();
void decode(t_instruccion *instruccion);
void execute(t_instruccion *instruccion);
void check_interrupt(t_instruccion *instruccion);

// -------------------- CICLO DE INSTRUCCION: Secundarias -------------------- //

// ---------- FETCH ---------- //

// Recibir string de memoria con instrucccion (id + parametros)
char *recibir_instruccion_string_memoria();

// Castear instruccion tipo string que llega de memoria.
t_instruccion *convertir_string_a_instruccion(char *instruccion_string);

// Setear id en EXIT por default y parametros en NULL
t_instruccion *inicializar_instruccion(t_instruccion *instruccion);

// Castear instruccion_id tipo string a enum para switch en execute(). Ante instruccion desconocida devuelve EXIT.
t_id string_id_to_enum_id(char *id_string);

// Liberar memoria dinámica (podría ser más bella si params[])
void destruir_instruccion(t_instruccion *instruccion);

// ---------- OTRAS ---------- //

bool instruccion_bloqueante(t_id id_instruccion);

// -------------------- INSTRUCCIONES -------------------- //

void set(char *nombre_registro, char *valor);
void sum(char *nombre_destino, char *nombre_origen);
void sub(char *nombre_destino, char *nombre_origen);
void jnz(char *nombre_registro, char *nro_instruccion);
void io_gen_sleep(char *nombre, char *unidades);
void exit_inst();

// -------------------- MANEJO DE CONTEXTO -------------------- //

void devolver_contexto(motivo_desalojo motivo_desalojo, t_list *param);

#endif
