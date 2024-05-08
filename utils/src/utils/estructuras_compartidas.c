#include "estructuras_compartidas.h"

// REGISTROS DE CPU

t_registros_cpu *crear_registros_cpu()
{
    t_registros_cpu *registros_cpu = malloc(sizeof(t_registros_cpu));
    if (registros_cpu != NULL)
    {
        registros_cpu->AX = 0;
        registros_cpu->BX = 0;
        registros_cpu->CX = 0;
        registros_cpu->DX = 0;
        registros_cpu->EAX = 0;
        registros_cpu->EBX = 0;
        registros_cpu->ECX = 0;
        registros_cpu->EDX = 0;
        registros_cpu->PC = 0;
        registros_cpu->SI = 0;
        registros_cpu->DI = 0;
    }
    return registros_cpu;
}

void destruir_registros_cpu(t_registros_cpu *registros_cpu)
{
    free(registros_cpu);
}

// PROVISORIO solicitud_de_instruccion

t_solicitud_de_instruccion *crear_solicitud_de_instruccion()
{

    t_solicitud_de_instruccion *solicitud_de_instruccion = malloc(sizeof(t_registros_cpu));
    if (solicitud_de_instruccion != NULL)
    {
        solicitud_de_instruccion->path = "";
        solicitud_de_instruccion->desplazamiento = 0;
    }
    return solicitud_de_instruccion;
}

void destruir_solicitud_de_instruccion(t_solicitud_de_instruccion *solicitud_de_instruccion)
{
    free(solicitud_de_instruccion);
}