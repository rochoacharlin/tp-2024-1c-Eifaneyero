#ifndef KERNEL_MAIN_H
#define KERNEL_MAIN_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

#define obtener_puerto_escucha() config_get_string_value(config, "PUERTO_ESCUCHA")
#define obtener_ip_memoria() config_get_string_value(config, "IP_MEMORIA")
#define obtener_puerto_memoria() config_get_string_value(config, "PUERTO_MEMORIA")
#define obtener_ip_cpu() config_get_string_value(config, "IP_CPU")
#define obtener_puerto_cpu_dispatch() config_get_string_value(config, "PUERTO_CPU_DISPATCH")
#define obtener_puerto_cpu_interrupt() config_get_string_value(config, "PUERTO_CPU_INTERRUPT")
#define obtener_algoritmo_planificacion() config_get_string_value(config, "ALGORITMO_PLANIFICACION")
#define obtener_quantum() config_get_int_value(config, "QUANTUM")
#define obtener_recursos() config_get_array_value(config, "RECURSOS")
#define obtener_instancias_recursos() config_get_array_value(config, "INSTANCIAS_RECURSOS")
#define obtener_grado_multiprogramacion() config_get_int_value(config, "GRADO_MULTIPROGRAMACION")

extern t_config *config;

#endif