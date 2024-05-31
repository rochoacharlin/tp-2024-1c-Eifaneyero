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

t_pcb *buscar_pcb_por_PID(t_list *lista_pcbs, uint32_t PID)
{
    t_pcb *pcb;
    for (int i = 0; i < list_size(lista_pcbs); i++)
    {
        pcb = list_get(lista_pcbs, i);
        if (pcb->PID == PID)
            return pcb;
    }

    return NULL;
}

void actualizar_pcb(t_pcb *proceso, t_contexto *contexto)
{
    destruir_registros_cpu(proceso->registros_cpu);
    contexto->registros_cpu = crear_registros_cpu();
    proceso->registros_cpu = copiar_registros_cpu(contexto->registros_cpu);
}

t_contexto *asignar_valores_pcb_a_contexto(t_pcb *proceso) // REVISAR !!!
{
    t_contexto *contexto = iniciar_contexto();
    contexto->PID = proceso->PID;
    contexto->registros_cpu = copiar_registros_cpu(proceso->registros_cpu);
    return contexto;
}