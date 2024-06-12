#ifndef GESTION_MEMORIA_H
#define GESTION_MEMORIA_H

#include <configuraciones.h>
#include "lectura/lectura.h"
#include "tablas/tablas.h"
#include <pthread.h>
#include <semaphore.h>

extern void *espacio_usuario;
extern t_dictionary *indice_tablas;
extern t_bitarray *marcos_libres;
extern t_dictionary *indice_instrucciones;
extern pthread_mutex_t mutex_memoria;
extern t_dictionary *sem_instrucciones_listas;

void inicializar_memoria(void);

void retardo_de_peticion();

sem_t *obtener_sem_instrucciones(uint32_t PID);

void escribir_espacio_usuario(uint32_t direccion, void *valor, uint32_t tamanio);
void recibir_escritura_espacio_usuario(int sockete, uint32_t *PID, uint32_t *direccion_fisica, void *valor_a_escribir, uint32_t *tamanio);
void atender_escritura_espacio_usuario(int sockete);

void enviar_valor_leido(int sockete, void *valor_leido, uint32_t tamanio_valor);
void *leer_espacio_usuario(uint32_t direccion, uint32_t tamanio_a_leer);
void recibir_lectura_espacio_usuario(int sockete, uint32_t *PID, uint32_t *direccion_fisica, uint32_t *tamanio_a_leer);
void atender_lectura_espacio_usuario(int sockete);

#endif