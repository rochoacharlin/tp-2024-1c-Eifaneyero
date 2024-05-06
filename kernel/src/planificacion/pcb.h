#ifndef KERNEL_PCB_H_
#define KERNEL_PCB_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "../configuraciones.h"
#include "../../../utils/src/utils/estructuras_compartidas.h"

typedef enum estado
{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT,
} estado;

typedef struct
{
    int PID;
    int PC; // no se si esto va, porque tambien esta en los registros de CPU
    int quantum;
    t_registros_cpu *registros_cpu;
    estado estado;
} t_pcb;

extern int32_t procesos_creados;
extern char *lista_PIDS;

t_pcb *crear_pcb();
void destruir_pcb(t_pcb *pcb);

t_pcb *desencolar_pcb(t_list *pcbs);
void encolar_pcb(t_list *pcbs, t_pcb *pcb);
void mostrar_PIDS(t_list *pcbs);
void agregar_PID_a_lista_PIDS(void *pcb);

#endif