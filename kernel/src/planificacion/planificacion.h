#ifndef KERNEL_PLANIFICACION_H
#define KERNEL_PLANIFICACION_H

#include <pthread.h>
#include <semaphore.h>
#include "pcb.h"
#include "../main.h"

extern sem_t hay_pcbs_READY;
extern pthread_mutex_t mutex_lista_READY;
extern sem_t sem_grado_multiprogramacion;

extern int grado_multiprogramacion;
extern int *instancias_recursos;

extern t_list *pcbs_en_READY;
extern char *estadosProcesos[5];

// largo plazo

// corto plazo
void planificar_a_corto_plazo_segun_algoritmo(void);
void planificar_a_corto_plazo(t_pcb *(*proximo_a_ejecutar)(void));
t_pcb *proximo_a_ejecutar_segun_FIFO(void);

void loggear_cambio_de_estado(int PID, estado anterior, estado actual);

#endif
