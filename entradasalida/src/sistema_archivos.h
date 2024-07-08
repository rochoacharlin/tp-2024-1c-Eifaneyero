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
extern t_list *fcbs;

typedef struct
{
    char *nombre;
    int bloque_inicial;
    int tamanio_en_bloques;
} t_fcb;

void crear_archivo(uint32_t *PID, char *nombre);
void eliminar_archivo(uint32_t *PID, char *nombre);
void truncar_archivo(uint32_t *PID, char *nombre, int tam);
void *leer_archivo(uint32_t *PID, char *nombre, int tam, int puntero);
void escribir_archivo(uint32_t *PID, char *nombre, int tam, int puntero);
void iniciar_bitmap();
void leer_bloques();
void leer_fcbs();
void cargar_fcb(t_fcb *fcb);
bool ordenar_fcb_por_bloque_inicial(void *fcb1, void *fcb2);

int bloque_inicial(char *archivo);
t_fcb *metadata_de_archivo(char *archivo);

#endif