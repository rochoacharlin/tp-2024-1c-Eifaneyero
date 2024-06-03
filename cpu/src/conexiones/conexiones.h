#ifndef CPU_CONEXIONES_H
#define CPU_CONEXIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include "../configuraciones.h"
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/contexto_ejecucion.h>

#include "../../cpu/src/ciclo_de_instruccion.h" //Compartir hay_interrupcion y motivo de instrucccion
//    #include <commons/collections/list.h>
//    #include <commons/collections/dictionary.h>

extern int conexion_cpu_memoria;
extern int conexion_cpu_kernel_dispatch;
extern int conexion_cpu_kernel_interrupt;
extern char *motivo_interrupcion;
extern bool hay_interrupcion;

void iniciar_servidor_dispatch(void);
void iniciar_servidor_interrupt(void);
void iniciar_conexion_con_memoria(void);

// Inicia conexiones kernel y memoria. Crea hilos para cada conexion con kernel.
void iniciar_conexiones();

void atender_dispatch();
void atender_interrupt();

char *recibir_interrupcion();

#endif
