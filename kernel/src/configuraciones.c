#include "configuraciones.h"

char *estados[5] = {"NEW", "READY", "EXEC", "BLOCKED", "EXIT"};
char *motivos[5] = {"SUCCESS", "INVALID_RESOURCE", "INVALID_INTERFACE", "OUT_OF_MEMORY", "INTERRUPTED_BY_USER"};

void loggear_creacion_proceso(int pcbPID)
{
    log_info(logger_obligatorio, "Se crea el proceso <%d> en NEW", pcbPID);
}

void loggear_fin_de_proceso(int PID, int motivo)
{
    log_info(logger_obligatorio, "Finaliza el proceso <%d> - Motivo: <%s>", PID, motivos[motivo]);
}

void loggear_cambio_de_estado(int PID, int anterior, int actual)
{
    log_info(logger_obligatorio, "PID: <%d> - Estado Anterior: <%s> - Estado Actual: <%s>", PID, estados[anterior], estados[actual]);
}

void loggear_motivo_de_bloqueo(int PID, char *interfaz_o_recurso)
{
    log_info(logger_obligatorio, "PID: <%d> - Bloqueado por: <%s>", PID, interfaz_o_recurso);
}

void loggear_fin_de_quantum(int PID)
{
    log_info(logger_obligatorio, "PID : <%d> - Desalojado por fin de Quantum", PID);
}

void loggear_ingreso_a_READY(char *lista_PIDS)
{
    log_info(logger_obligatorio, "Cola Ready / Ready Prioridad: [%s]", lista_PIDS);
}
