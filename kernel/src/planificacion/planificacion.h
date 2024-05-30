#ifndef KERNEL_PLANIFICACION_H
#define KERNEL_PLANIFICACION_H

#include <pthread.h>
#include <semaphore.h>
#include "pcb.h"
#include "manejo_interfaces.h"
#include "manejo_recursos.h"
#include <string.h>
#include "../configuraciones.h"

extern t_list *pcbs_en_NEW;
extern t_list *pcbs_en_memoria;
extern t_list *pcbs_en_READY;
extern t_list *pcbs_en_EXEC;
extern t_list *pcbs_en_BLOCKED;
extern t_list *pcbs_en_EXIT;

extern sem_t sem_grado_multiprogramacion;
extern int grado_multiprogramacion;

extern int conexion_kernel_cpu_dispatch;
extern int conexion_kernel_cpu_interrupt;

extern sem_t hay_pcbs_NEW;
extern pthread_mutex_t mutex_lista_NEW;
extern pthread_mutex_t mutex_lista_READY;
extern sem_t planificacion_liberada;
extern sem_t hay_pcbs_READY;

extern int *instancias_recursos;
extern char *estadosProcesos[5];
extern int conexion_kernel_cpu_dispatch;
extern int conexion_kernel_cpu_interrupt;

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
t_pcb *proximo_a_ejecutar_segun_FIFO_o_RR(void);

// relacionado con la CPU
t_contexto *procesar_pcb_segun_algoritmo(t_pcb *pcb);
t_contexto *ejecutar_segun_FIFO(t_contexto *contexto);
t_contexto *ejecutar_segun_RR_o_VRR(t_contexto *contexto);
void enviar_interrupcion_FIN_Q(int PID, int fd);
t_paquete *crear_paquete_interrupcion(int PID)

#endif
