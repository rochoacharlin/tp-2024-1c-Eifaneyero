#ifndef GESTION_MEMORIA_H
#define GESTION_MEMORIA_H

#include <configuraciones.h>
#include "lectura/lectura.h"
#include "tablas/tablas.h"

extern void *espacio_usuario;
extern t_dictionary *indice_tablas;
extern t_bitarray *marcos_libres;

void inicializar_memoria(void);

#endif