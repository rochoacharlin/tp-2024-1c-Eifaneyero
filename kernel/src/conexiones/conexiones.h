#ifndef KERNEL_CONEXIONES_H
#define KERNEL_CONEXIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../configuraciones.h"
#include "../planificacion/pcb.h"
#include "utils/comunicacion.h"
#include "utils/serializacion.h"
#include "utils/contexto_ejecucion.h"
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

extern int conexion_kernel_cpu_dispatch;  // socket
extern int conexion_kernel_cpu_interrupt; // socket

void servidor(void);
void conexion_dispatch_con_CPU(void);
void conexion_interrupt_con_CPU(void);
void iniciar_conexion_memoria(void);

// Envío contexto a CPU a partir de PCB y quedo a espera de OpCode
// Luego recibo contexto actualizado y actualizo PCB del proceso
t_contexto *gestionar_ejecucion_proceso(t_pcb *proceso_en_ejecucion);

#endif
