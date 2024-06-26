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

void crear_archivo(uint32_t *PID, char *nombre);
void eliminar_archivo(uint32_t *PID, char *nombre);
void truncar_archivo(uint32_t *PID, char *nombre, int tam);
void leer_archivo(uint32_t *PID, char *nombre, int tam, int *puntero);
void escribir_archivo(uint32_t *PID, char *nombre, int tam, int *puntero);

#endif