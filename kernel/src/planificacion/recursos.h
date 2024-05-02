#ifndef KERNEL_RECURSOS_H
#define KERNEL_RECURSOS_H

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include "../main.h"

extern int *instancias_recursos;
extern t_list *recursos;
extern char **nombres_recursos;

void crear_colas_de_bloqueo(void);
void destruir_lista_string(char **lista_string);

#endif