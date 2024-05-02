#include "planificacion.h"

t_list *pcbs_en_READY;
int32_t procesos_creados = 0;
int grado_multiprogramacion;

char *estados[5] = {"NEW", "READY", "EXEC", "BLOCKED", "EXIT"};

void planificar_a_corto_plazo_segun_algoritmo(void)
{
    char *algoritmo = obtener_algoritmo_planificacion();

    if (strcmp(algoritmo, "FIFO") == 0)
    {
        planificar_a_corto_plazo(proximo_a_ejecutar_segun_FIFO);
    }
    else if (strcmp(algoritmo, "RR") == 0)
    {
        // Solucion RR
    }
    else if (strcmp(algoritmo, "VRR") == 0)
    {
        // Solucion VRR
    }
    else
    {
        log_error(logger, "Algoritmo invalido. Debe ingresar FIFO, RR o VRR");
        abort();
    }
}

void planificar_a_corto_plazo(t_pcb *(*proximo_a_ejecutar)())
{
    // crear_colas_de_bloqueo();
    while (1)
    {
        // sem_wait(&hay_pcbs_READY);
        t_pcb *pcb_proximo = proximo_a_ejecutar();

        estado anterior = pcb_proximo->estado;
        pcb_proximo->estado = EXEC;

        loggear_cambio_de_estado(pcb_proximo->PID, anterior, pcb_proximo->estado);

        // contexto_ejecucion = procesar_pcb(pcb_proximo);
        // rafaga_CPU = contexto_ejecucion->rafaga_CPU_ejecutada;
        // retorno_contexto(pcb_proximo, contexto_ejecucion);
    }
}

t_pcb *proximo_a_ejecutar_segun_FIFO(void)
{
    return desencolar_pcb(pcbs_en_READY);
}

void loggear_cambio_de_estado(int PID, estado anterior, estado actual)
{
    log_info(logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>", PID, estados[anterior], estados[actual]);
}