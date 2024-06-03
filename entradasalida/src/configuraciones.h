#ifndef ENTRADASALIDA_CONFIGURACIONES_H
#define ENTRADASALIDA_CONFIGURACIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <commons/string.h>
#include "../../cpu/src/ciclo_de_instruccion.h"

#define obtener_tipo_interfaz() config_get_string_value(config, "TIPO_INTERFAZ")
#define obtener_tiempo_unidad_trabajo() config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO")
#define obtener_ip_kernel() config_get_string_value(config, "IP_KERNEL")
#define obtener_puerto_kernel() config_get_string_value(config, "PUERTO_KERNEL")
#define obtener_ip_memoria() config_get_string_value(config, "IP_MEMORIA")
#define obtener_puerto_memoria() config_get_string_value(config, "PUERTO_MEMORIA")
#define obtener_path_base_dialfs() config_get_string_value(config, "PATH_BASE_DIALFS")
#define obtener_block_size() config_get_int_value(config, "BLOCK_SIZE")
#define obtener_block_count() config_get_int_value(config, "BLOCK_COUNT")
#define obtener_retraso_compactacion() config_get_int_value(config, "RETRASO_COMPACTACION")

extern t_config *config;
extern t_log *logger_propio;
extern t_log *logger_obligatorio;

// Logs minimos y obligatorios
void loggear_operacion(int PID, char *operacion);
void loggear_dialfs_crear_archivo(int PID, char *nombre);
void loggear_dialfs_eliminar_archivo(int PID, char *nombre);
void loggear_dialfs_truncar_archivo(int PID, char *nombre, int tamanio);
void loggear_dialfs_leer_archivo(int PID, char *nombre, int tamanio, int puntero);
void loggear_dailfs_escribir_archivo(int PID, char *nombre, int tamanio, int puntero);
void loggear_dailfs_inicio_compactacion(int PID);
void loggear_dailfs_fin_compactacion(int PID);

#endif