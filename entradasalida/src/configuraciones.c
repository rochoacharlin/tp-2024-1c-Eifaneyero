#include "configuraciones.h"

void loggear_operacion(int PID, char *operacion)
{
    log_info(logger_obligatorio, "PID: <%d> - Operacion: <%s>", PID, operacion);
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

void loggear_dialfs_escribir_archivo(int PID, char *nombre, int tamanio, int puntero)
{
    // TODO
}

void loggear_dialfs_inicio_compactacion(int PID)
{
    // TODO
}

void loggear_dialfs_fin_compactacion(int PID)
{
    // TODO
}