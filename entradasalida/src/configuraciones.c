#include "configuraciones.h"

void loggear_operacion(int PID, char *operacion)
{
    log_info(logger_obligatorio, "PID: <%d> - Operacion: <%s>", PID, operacion);

    // TODO
}

void loggear_dialfs_crear_archivo(int PID, char *nombre)
{
    // TODO
}

void loggear_dialfs_eliminar_archivo(int PID, char *nombre)
{
    // TODO
}

void loggear_dialfs_truncar_archivo(int PID, char *nombre, int tamanio)
{
    // TODO
}

void loggear_dialfs_leer_archivo(int PID, char *nombre, int tamanio, int puntero)
{
    // TODO
}

void loggear_dailfs_escribir_archivo(int PID, char *nombre, int tamanio, int puntero)
{
    // TODO
}

void loggear_dailfs_inicio_compactacion(int PID)
{
    // TODO
}

void loggear_dailfs_fin_compactacion(int PID)
{
    // TODO
}
