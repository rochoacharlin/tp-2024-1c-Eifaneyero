#ifndef MEMORIA_MAIN_H
#define MEMORIA_MAIN_H

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

#endif