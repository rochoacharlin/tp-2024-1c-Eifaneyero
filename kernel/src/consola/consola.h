#ifndef KERNEL_CONSOLA_H_
#define KERNEL_CONSOLA_H_

// #include <string.h>
#include <readline/readline.h>
#include <stdlib.h>
#include "../main.h"
#include "../planificacion/pcb.h"

extern t_list *pcbs_en_READY;

void consola(void);
void listar_procesos_por_cada_estado(void);
void listar_procesos_por_estado(char *estado, t_list lista);

#endif