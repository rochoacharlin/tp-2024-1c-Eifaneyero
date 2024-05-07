#ifndef CPU_INTERFACE_CPU_H
#define CPU_INTERFACE_CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>
#include <utils/estructuras_compartidas.h>

void solicitar_instruccion(int conexion_memoria, char *direccion_instruciones, uint32_t desplazamiento);

#endif