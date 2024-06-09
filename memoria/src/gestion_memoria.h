#ifndef GESTION_MEMORIA_H
#define GESTION_MEMORIA_H

#include <commons/collections/dictionary.h>
#include <commons/bitarray.h>
#include <configuraciones.h>

extern void *espacio_usuario;
extern t_dictionary *indice_tablas;
extern t_bitarray *marcos_disponibles;

void inicializar_memoria(void);
void crear_marcos_memoria(int cantidad);

#endif