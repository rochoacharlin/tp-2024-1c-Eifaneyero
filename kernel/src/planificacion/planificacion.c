#include "planificacion.h"

void planificar_a_corto_plazo_segun_algoritmo()
{
    char *algoritmo = obtener_algoritmo_planificacion();

    if (strcmp(algoritmo, "FIFO") == 0)
    {
        // Solucion FIFO
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