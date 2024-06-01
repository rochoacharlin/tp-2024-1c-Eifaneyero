#ifndef KERNEL_RECURSOS_E_INTERFACES_H
#define KERNEL_RECURSOS_E_INTERFACES_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include "../configuraciones.h"
#include <utils/estructuras_compartidas/estructuras_compartidas.h>
#include "../planificacion/pcb.h"

extern int *instancias_recursos;
extern t_list *recursos;
extern char **nombres_recursos;

void crear_colas_de_bloqueo(void);
void destruir_lista_string(char **lista_string);

#endif