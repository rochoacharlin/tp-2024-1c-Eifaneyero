#ifndef ENTRADASALIDA_SISTEMA_ARCHIVOS_H
#define ENTRADASALIDA_SISTEMA_ARCHIVOS_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <dirent.h>
#include "configuraciones.h"

extern t_config *metadata;
extern int bloque_utilizados;
extern t_bitarray *bitmap;

typedef struct
{
    char *nombre;
    int bloque_inicial;
    int tamanio_en_bloques;
} t_fcb;

void crear_archivo(uint32_t *PID, char *nombre);
void eliminar_archivo(uint32_t *PID, char *nombre);
void truncar_archivo(uint32_t *PID, char *nombre, int tam);
void leer_archivo(uint32_t *PID, char *nombre, int tam, int *puntero);
void escribir_archivo(uint32_t *PID, char *nombre, int tam, int *puntero);
void iniciar_bitmap();
void leer_bloques();
void leer_fcbs();

#endif