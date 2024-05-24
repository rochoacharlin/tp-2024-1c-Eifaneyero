#ifndef CPU_CONEXIONES_H
#define CPU_CONEXIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../configuraciones.h"
#include "utils/comunicacion.h"
// #include "utils/serializacion.h"
// #include "utils/contexto_ejecucion.h"
// #include <commons/collections/list.h>
// #include <commons/collections/dictionary.h>

extern int conexion_cpu_memoria;

void servidor_dispatch(void);
void servidor_interrupt(void);
int conexion_con_memoria(void);

#endif
