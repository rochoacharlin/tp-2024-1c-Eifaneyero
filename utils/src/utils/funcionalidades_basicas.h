#ifndef UTILS_FUNCIONALIDADES_BASICAS_H_
#define UTILS_FUNCIONALIDADES_BASICAS_H_

#include <stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#define SERVIDOR 0
#define C_CPU 1
#define C_MEMORIA 2

t_log *crear_logger(char *modulo);
t_config *iniciar_config(t_log *logger, char *ruta_archivo);

#endif
