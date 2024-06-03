#ifndef CPU_INTERFACE_CPU_H
#define CPU_INTERFACE_CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/contexto_ejecucion.h>
#include "conexiones/conexiones.h"

void solicitar_lectura_de_instruccion(uint32_t desplazamiento);

#endif