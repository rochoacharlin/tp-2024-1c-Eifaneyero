#ifndef CPU_CONFIGURACIONES_H
#define CPU_CONFIGURACIONES_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

#define obtener_ip_memoria() config_get_string_value(config, "IP_MEMORIA")
#define obtener_puerto_memoria() config_get_string_value(config, "PUERTO_MEMORIA")
#define obtener_puerto_escucha_dispatch() config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH")
#define obtener_puerto_escucha_interrupt() config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT")
#define obtener_cantidad_entradas_tlb() config_get_int_value(config, "CANTIDAD_ENTRADAS_TLB")
#define obtener_algoritmo_tlb() config_get_string_value(config, "ALGORITMO_TLB")

extern t_config *config;
extern t_log *logger;

// Logs minimos y obligatorios
void loggear_fetch_instrucccion(int PID, int PC);
void loggear_instrucion_ejecutada(int PID, int instruccion, int parametros); // ni idea si estos son los tipos de datos correctos
void loggear_tlb_hit(int PID, int numero_pagina);
void loggear_tlb_miss(int PID, int numero_pagina);
void loggear_obtener_marco(int PID, int numero_pagina, int numero_marco);
void loggear_lectura_o_escritura_memoria(int PID, char accion, int direccion_fisica, int valor);

#endif