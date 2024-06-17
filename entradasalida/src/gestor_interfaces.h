#ifndef ENTRADASALIDA_GESTOR_INTERFACES_H
#define ENTRADASALIDA_GESTOR_INTERFACES_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <commons/string.h>
#include <utils/estructuras_compartidas/instrucciones_cpu.h>
#include "configuraciones.h"

extern int conexion_memoria;
extern op_code (*atender)(int cod_op, t_list *parametros);

void atender_segun_tipo_interfaz(void);
uint32_t calcular_tiempo_de_espera(int unidades_de_trabajo);
op_code atender_gen(int cod_op, t_list *parametros);
op_code atender_stdin(int cod_op, t_list *parametros);
op_code atender_stdout(int cod_op, t_list *parametros);
op_code atender_dialfs(int cod_op, t_list *parametros);

void setear_config(char *archivo_config_io);

#endif