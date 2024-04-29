#ifndef KERNEL_PCB_H_
#define KERNEL_PCB_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum estado_proceso
{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    EXIT,
} estado_proceso;

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
    estado_proceso estado;
} t_pcb;

int32_t procesos_creados = 0;

t_pcb *crear_pcb();
t_registros_cpu *crear_registros_cpu(); // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU
void destruir_pcb(t_pcb *pcb);
void destruir_registros_cpu(t_registros_cpu *registros_cpu); // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU

#endif