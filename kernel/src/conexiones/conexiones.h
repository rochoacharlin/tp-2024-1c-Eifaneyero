#ifndef KERNEL_CONEXIONES_H
#define KERNEL_CONEXIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../configuraciones.h"
#include "../planificacion/pcb.h"
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/estructuras_compartidas.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

extern int conexion_kernel_cpu_dispatch;
extern int conexion_kernel_cpu_interrupt;
extern int conexion_kernel_memoria;
extern int servidor_kernel_fd;

void servidor(void);
void conexion_dispatch_con_CPU(void);
void conexion_interrupt_con_CPU(void);
void conexion_memoria(void);

// Envío contexto a CPU a partir de PCB y quedo a espera de OpCode
// Luego recibo contexto actualizado y actualizo PCB del proceso
t_contexto *gestionar_ejecucion_proceso(t_pcb *proceso_en_ejecucion, t_contexto *contexto_ejecucion);

#endif
