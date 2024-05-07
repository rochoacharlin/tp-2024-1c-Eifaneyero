#ifndef MEMORIA_CONFIGURACIONES_H
#define MEMORIA_CONFIGURACIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

#define obtener_puerto_escucha() config_get_string_value(config, "PUERTO_ESCUCHA")
#define obtener_tam_memoria() config_get_int_value(config, "TAM_MEMORIA")
#define obtener_tam_pagina() config_get_int_value(config, "TAM_MEMORIA")
#define obtener_path_instrucciones() config_get_string_value(config, "PATH_INSTRUCCIONES")
#define obtener_retardo_respuesta() config_get_int_value(config, "RETARDO_RESPUESTA")

extern t_config *config;
extern t_log *logger;

// Logs minimos y obligatorios
void loggear_creacion_destruccion_tabla_de_paginas(int PID, int cantidad_paginas);
void loggear_acceso_tablas_de_paginas(int PID, int pagina, int marco);
void loggear_ampliacion_de_proceso(int PID, int tamanio_actual, int tamanio_a_ampliar);
void loggear_reduccion_de_proceso(int PID, int tamanio_actual, int tamanio_a_reducir);
void loggear_acceso_a_espacio_de_usuario(int PID, int accion, int direccion_fisica, int tamanio);

#endif