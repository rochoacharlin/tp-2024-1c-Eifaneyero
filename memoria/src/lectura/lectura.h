#ifndef MEMORIA_LECTURA_H
#define MEMORIA_LECTURA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configuraciones.h"
#include <utils/estructuras_compartidas/contexto_ejecucion.h>

extern t_log *logger_obligatorio;
extern t_log *logger_propio;

t_instruccion_cadena *leer_instruccion(int desplazamiento);

#endif