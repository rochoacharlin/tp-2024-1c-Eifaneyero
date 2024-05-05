#ifndef KERNEL_PLANIFICACION_H
#define KERNEL_PLANIFICACION_H

#include <pthread.h>
#include <semaphore.h>
#include "pcb.h"
#include "recursos.h"

extern sem_t sem_grado_multiprogramacion;
extern int grado_multiprogramacion;
extern t_list *pcbs_en_NEW;
extern sem_t hay_pcbs_NEW;
extern pthread_mutex_t mutex_lista_NEW;
extern pthread_mutex_t mutex_lista_READY;
extern t_list *pcbs_en_memoria;
extern sem_t planificacion_liberada;

extern sem_t hay_pcbs_READY;
extern int *instancias_recursos;

extern t_list *pcbs_en_READY;
extern char *estadosProcesos[5];

// largo plazo
void planificar_a_largo_plazo(void);
void ingresar_pcb_a_NEW(t_pcb *pcb);
t_pcb *obtener_siguiente_pcb_READY(void);
void ingresar_pcb_a_READY(t_pcb *pcb);
void inicializar_listas_planificacion(void);
void destruir_listas_planificacion(void);

// manejo de semaforos
void inicializar_semaforos_planificacion(void);
void destruir_semaforos_planificacion(void);

// corto plazo
void planificar_a_corto_plazo_segun_algoritmo(void);
void planificar_a_corto_plazo(t_pcb *(*proximo_a_ejecutar)(void));
t_pcb *proximo_a_ejecutar_segun_FIFO(void);

// loggeos minimos y necesarios
void loggear_cambio_de_estado(int PID, estado anterior, estado actual);
void loggear_ingreso_a_READY(char *lista_PIDS);
void loggear_creacion_proceso(t_pcb *pcb);

#endif
