#ifndef GESTION_MEMORIA_H
#define GESTION_MEMORIA_H

#include <configuraciones.h>
#include "lectura/lectura.h"
#include "tablas/tablas.h"

extern void *espacio_usuario;
extern t_dictionary *indice_tablas;
extern t_bitarray *marcos_libres;
extern t_dictionary *indice_instrucciones;

void inicializar_memoria(void);

uint32_t leer_espacio_usuario(uint32_t direccion);
void escribir_espacio_usuario(uint32_t direccion, uint32_t valor);

#endif