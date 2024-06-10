#include "manejo_recursos.h"

int *instancias_recursos;
t_list *colas_de_recursos;
char **nombres_recursos;

void crear_colas_de_bloqueo(void)
{
    instancias_recursos = NULL;
    colas_de_recursos = list_create();
    nombres_recursos = obtener_recursos();
    char **instancias_aux = obtener_instancias_recursos();

    for (int i = 0; i < string_array_size(instancias_aux); i++)
    {
        int instancia_en_entero = atoi(instancias_aux[i]);
        instancias_recursos = realloc(instancias_recursos, (i + 1) * sizeof(int));
        instancias_recursos[i] = instancia_en_entero;

        t_list *cola_bloqueo = list_create();
        list_add(colas_de_recursos, (void *)cola_bloqueo);
    }

    destruir_lista_string(instancias_aux);
}

void wait_recurso(char *recurso, t_pcb *pcb)
{
    if (existe_recurso(recurso))
    {
        int cantidad_instancias_recurso = --instancias_recursos[posicion_recurso(recurso)];
        if (cantidad_instancias_recurso < 0)
        {
            pcb->estado = BLOCKED;
            pcb_en_EXEC = NULL;
            // VERIFICAR: Hace falta un mutex para blocked?
            list_add(pcbs_en_BLOCKED, pcb);

            // logs minimos y obligatorios
            loggear_cambio_de_estado(pcb->PID, EXEC, BLOCKED);
            loggear_motivo_de_bloqueo(pcb->PID, recurso);

            // se queda bloqueado en la lista correspondiente al recurso
            t_list *cola_bloqueo_recurso = list_get(colas_de_recursos, posicion_recurso(recurso));
            list_add(cola_bloqueo_recurso, pcb);
        }
        else
        {
            list_add(pcb->recursos_asignados, recurso);
        }
    }
    else
    {
        enviar_pcb_a_EXIT(pcb, INVALID_RESOURCE);
    }
}

void signal_recurso(char *recurso, t_pcb *pcb, int rafaga_cpu_ejecutada)
{
    bool condicion_liberar_recurso(void *elemento)
    {
        return strcmp(recurso, (char *)elemento) == 0;
    }

    if (existe_recurso(recurso))
    {
        int cantidad_instancias_recurso = ++instancias_recursos[posicion_recurso(recurso)];
        if (cantidad_instancias_recurso == 0) // REVISAR: es correcta la condicion?
        {
            // desbloqueamos al primer proceso de la cola de bloqueados de ese recurso
            t_list *cola_bloqueo_recurso = list_get(colas_de_recursos, posicion_recurso(recurso));
            t_pcb *pcb_a_desbloquear = desencolar_pcb(cola_bloqueo_recurso);
            pcb_a_desbloquear->estado = READY;

            // log minimo y obligatorio
            loggear_cambio_de_estado(pcb_a_desbloquear->PID, BLOCKED, READY);

            encolar_pcb_ready_segun_algoritmo(pcb_a_desbloquear, rafaga_cpu_ejecutada);

            // devolvemos la ejecucion al pcb
            procesar_pcb_segun_algoritmo(pcb);
            esperar_contexto_y_actualizar_pcb(pcb);
        }
        else
        {
            // VERIFICAR: Funciona esto?
            list_remove_by_condition(pcb->recursos_asignados, condicion_liberar_recurso);
        }
    }
    else
    {
        enviar_pcb_a_EXIT(pcb, INVALID_RESOURCE);
    }
}

void destruir_lista_string(char **lista_string)
{
    for (int i = 0; i < string_array_size(lista_string); i++)
        free(lista_string[i]);
    free(lista_string);
}

bool existe_recurso(char *recurso)
{
    for (int i = 0; i < cantidad_recursos(); i++)
    {
        if (strcmp(nombres_recursos[i], recurso) == 0)
            return true;
    }
    return false;
}

int posicion_recurso(char *recurso)
{
    for (int i = 0; i < cantidad_recursos(); i++)
    {
        if (strcmp(nombres_recursos[i], recurso) == 0)
            return i;
    }
    return 0;
}

int cantidad_recursos(void)
{
    int cantidad = 0;
    while (nombres_recursos[cantidad] != NULL)
        cantidad++;

    return cantidad;
}