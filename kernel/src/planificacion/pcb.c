#include "pcb.h"

char *lista_PIDS;

t_pcb *crear_pcb()
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->PID = procesos_creados++; // le asigno un PID y luego lo incremento
    pcb->PC = 0;                   // no se si esto va, porque tambien esta en los registros de CPU
    pcb->quantum = obtener_quantum();
    pcb->registros_cpu = crear_registros_cpu();
    pcb->estado = NEW;

    return pcb;
}

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

void destruir_pcb(t_pcb *pcb)
{
    destruir_registros_cpu(pcb->registros_cpu);
    free(pcb);
}

void destruir_registros_cpu(t_registros_cpu *registros_cpu)
{
    free(registros_cpu);
}

t_pcb *desencolar_pcb(t_list *pcbs)
{
    return (t_pcb *)list_remove(pcbs, 0);
}

void encolar_pcb(t_list *pcbs, t_pcb *pcb)
{
    list_add(pcbs, (void *)pcb);
}

void mostrar_PIDS(t_list *lista_pcbs)
{
    list_iterate(lista_pcbs, agregar_PID_a_lista_PIDS);
}

void agregar_PID_a_lista_PIDS(void *valor)
{
    t_pcb *pcb = (t_pcb *)valor;
    char *PID = string_itoa(pcb->PID);
    string_append_with_format(&lista_PIDS, " %s ", PID);
    free(PID);
}