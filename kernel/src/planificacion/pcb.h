#ifndef KERNEL_PCB_H_
#define KERNEL_PCB_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/collections/list.h>
#include "../main.h"

typedef enum estado
{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT,
} estado;

typedef struct // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU
{
    uint8_t AX, BX, CX, DX;
    uint32_t EAX, EBX, ECX, EDX, PC, SI, DI;
} t_registros_cpu;

typedef struct
{
    int PID;
    int PC; // no se si esto va, porque tambien esta en los registros de CPU
    int quantum;
    t_registros_cpu *registros_cpu;
    estado estado;
} t_pcb;

extern int32_t procesos_creados;
extern t_list *pcbs_en_READY;

t_pcb *crear_pcb();
t_registros_cpu *crear_registros_cpu(); // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU
void destruir_pcb(t_pcb *pcb);
void destruir_registros_cpu(t_registros_cpu *registros_cpu); // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU

t_pcb *desencolar_pcb(t_list *pcbs);
void encolar_pcb(t_list *pcbs, t_pcb *pcb);

#endif