#ifndef CICLO_DE_INSTRUCCION_H
#define CICLO_DE_INSTRUCCION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
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
#include "mmu/mmu.h"

// ID instruccion

typedef struct
{
    t_id id;
    char *param1;
    char *param2;
    char *param3;
    char *param4;
    char *param5;
    t_list *direcciones_fisicas;
} t_instruccion;

extern t_TLB *tlb;

// Version 2.0: Posiblemente quedaba mejor
// typedef struct {
//     t_id id;
//     char* parametros[];
// }t_instruccion;

// extern pthread_mutex_t mutex_interrupt;

// -------------------- CICLO DE INSTRUCCION -------------------- //

void ciclo_de_instruccion(t_contexto *contexto);
char *fetch();
t_instruccion *decode(char *instruccion);
void execute(t_instruccion *instruccion);
void check_interrupt(t_instruccion *instruccion);

// -------------------- CICLO DE INSTRUCCION: Secundarias -------------------- //

// ---------- FETCH ---------- //

// Recibir string de memoria con instrucccion (id + parametros)
char *recibir_instruccion_string();

// Castear instruccion tipo string que llega de memoria.
t_instruccion *convertir_string_a_instruccion(char *instruccion_string);

// Setear id en EXIT por default y parametros en NULL
t_instruccion *inicializar_instruccion(t_instruccion *instruccion);

// Castear instruccion_id tipo string a enum para switch en execute(). Ante instruccion desconocida devuelve EXIT.
t_id string_id_to_enum_id(char *id_string);

// Liberar memoria dinámica (podría ser más bella si params[])
void destruir_instruccion(t_instruccion *instruccion);

// ---------- DECODE ---------- //

uint8_t tamanio_de_registro(char *registro);

// ---------- OTRAS ---------- //

bool instruccion_bloqueante(t_id id_instruccion);

void enviar_lectura_espacio_usuario(uint32_t PID, void *direccion, int *tamanio);
void enviar_escritura_espacio_usuario(uint32_t PID, void *direccion, void *valor_a_escribir, int *tamanio);
void agregar_direcciones_fisicas(t_instruccion *instruccion, uint32_t direccion_logica, uint_8_t tamanio_a_operar);

// -------------------- INSTRUCCIONES -------------------- //

void set(char *nombre_registro, char *valor);
void sum(char *nombre_destino, char *nombre_origen);
void sub(char *nombre_destino, char *nombre_origen);
void jnz(char *nombre_registro, char *nro_instruccion);
void io_gen_sleep(char *nombre, char *unidades);
void io_stdin_read(char *nombre, t_list *direcciones_fisicas, char *registro_tamanio);
void io_stdout_write(char *nombre, t_list *direcciones_fisicas, char *registro_tamanio);
void mov_in(char *registro_datos_destino, t_list *direcciones_fisicas);
void mov_out(char *registro_datos, t_list *direcciones_fisicas);
void resize(uint32_t tamanio);
void copy_string(t_list *direcciones_fisicas);
void exit_inst();

// -------------------- MANEJO DE CONTEXTO -------------------- //

void devolver_contexto(motivo_desalojo motivo_desalojo, t_list *param);

#endif
