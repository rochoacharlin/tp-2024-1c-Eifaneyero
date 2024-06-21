#ifndef KERNEL_RECURSOS_E_INTERFACES_H
#define KERNEL_RECURSOS_E_INTERFACES_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include "../configuraciones.h"
#include "../planificacion/pcb.h"
#include "../planificacion/planificacion.h"

extern int *instancias_recursos;
extern t_list *colas_de_recursos;
extern char **nombres_recursos;

void crear_colas_de_bloqueo(void);
void destruir_lista_string(char **lista_string);
void wait_recurso(char *recurso, t_pcb *pcb);
void signal_recurso(char *recurso, t_pcb *pcb);
bool existe_recurso(char *recurso);
int posicion_recurso(char *recurso);
int cantidad_recursos(void);
void liberar_recursos(t_pcb *pcb);

#endif