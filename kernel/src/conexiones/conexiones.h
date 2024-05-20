#ifndef KERNEL_CONEXIONES_H
#define KERNEL_CONEXIONES_H

#include "configuraciones.h"
#include <utils/comunicacion.h>

void servidor(void);
void conexion_dispatch_con_CPU(void);
void conexion_interrupt_con_CPU(void);
void conexion_con_memoria(void);

#endif