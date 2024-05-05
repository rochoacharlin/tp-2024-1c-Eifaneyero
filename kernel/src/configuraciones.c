#include "configuraciones.h"

char *estados[5] = {"NEW", "READY", "EXEC", "BLOCKED", "EXIT"};

void loggear_cambio_de_estado(int PID, int anterior, int actual)
{
    log_info(logger, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>", PID, estados[anterior], estados[actual]);
}

void loggear_ingreso_a_READY(char *lista_PIDS)
{
    log_info(logger, "Cola Ready <%s>: [%s]", obtener_algoritmo_planificacion(), lista_PIDS);
}

void loggear_creacion_proceso(int pcbPID)
{
    log_info(logger, "Se crea el proceso <%d> en NEW", pcbPID);
}