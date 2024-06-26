#ifndef ENTRADASALIDA_SISTEMA_ARCHIVOS_H
#define ENTRADASALIDA_SISTEMA_ARCHIVOS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include "configuraciones.h"

extern FILE *bloques;
extern FILE *bitmap;
extern t_config *metadata;
extern int bloque_utilizados;
extern t_bitarray *bitarray;

void crear_archivo(char *nombre);
void eliminar_archivo(char *nombre);
void truncar_archivo(char *nombre);
void leer_archivo(char *nombre);
void escribir_archivo(char *nombre);

#endif