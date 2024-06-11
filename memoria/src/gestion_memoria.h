#ifndef GESTION_MEMORIA_H
#define GESTION_MEMORIA_H

#include <configuraciones.h>
#include "lectura/lectura.h"
#include "tablas/tablas.h"
#include <pthread.h>

extern void *espacio_usuario;
extern t_dictionary *indice_tablas;
extern t_bitarray *marcos_libres;
extern t_dictionary *indice_instrucciones;
extern pthread_mutex_t mutex_memoria;

void inicializar_memoria(void);

#endif