#ifndef MEMORIA_TABLAS_H
#define MEMORIA_TABLAS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <utils/funcionalidades_basicas.h>
#include <commons/collections/list.h>

#include "tablas.c"

extern int tamanio_pagina;

// INDICE DE TABLA DE PAGINAS
t_dictionary *crear_indice_de_tablas();
void agregar_proceso_al_indice(t_dictionary *indice_de_tablas, uint32_t PID, int *tabla_de_paginas);
void quitar_proceso_del_indice(t_dictionary *indice_de_tablas, uint32_t PID);
int *obtener_tabla_paginas(t_dictionary *indice_de_tablas, uint32_t PID);
void destruir_indice_de_tablas(t_dictionary *indice_de_tablas);

#endif