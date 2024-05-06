#ifndef UTILS_ESTRUCTURAS_PLANIFICACION_H_
#define UTILS_ESTRUCTURAS_PLANIFICACION_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>
#include <commons/collections/list.h>

typedef struct // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU
{
    uint8_t AX, BX, CX, DX;
    uint32_t EAX, EBX, ECX, EDX, PC, SI, DI;
} t_registros_cpu;

t_registros_cpu *crear_registros_cpu();                      // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU
void destruir_registros_cpu(t_registros_cpu *registros_cpu); // supongo que despues esto va a estar en utils ya que lo utiliza tanto el Kernel como la CPU

#endif