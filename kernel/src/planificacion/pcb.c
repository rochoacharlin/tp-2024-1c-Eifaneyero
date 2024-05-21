#include "pcb.h"

char *lista_PIDS;

t_pcb *crear_pcb()
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->PID = procesos_creados++; // le asigno un PID y luego lo incremento
    pcb->quantum = obtener_quantum();
    pcb->registros_cpu = crear_registros_cpu();
    pcb->estado = NEW;

    return pcb;
}

void destruir_pcb(t_pcb *pcb)
{
    destruir_registros_cpu(pcb->registros_cpu);
    free(pcb);
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