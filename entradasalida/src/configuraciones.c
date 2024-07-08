#include "configuraciones.h"

void loggear_operacion(int PID, char *operacion)
{
    log_info(logger_obligatorio, "PID: <%d> - Operacion: <%s>", PID, operacion);
}

void loggear_dialfs_crear_archivo(int PID, char *nombre)
{
    log_info(logger_obligatorio, "PID: <%d> - Crear archivo: <%s>", PID, nombre);
}

void loggear_dialfs_eliminar_archivo(int PID, char *nombre)
{
    log_info(logger_obligatorio, "PID: <%d> - Eliminar archivo: <%s>", PID, nombre);
}

void loggear_dialfs_truncar_archivo(int PID, char *nombre, int tam)
{
    log_info(logger_obligatorio, "PID: <%d> - Truncar archivo: <%s> - Tamaño: <%d>", PID, nombre, tam);
}

void loggear_dialfs_leer_archivo(int PID, char *nombre, int tam, int puntero)
{
    log_info(logger_obligatorio, "PID: <%d> - Leer Archivo: <%s> - Tamaño a Leer: <%d> - Puntero Archivo: <%d>", PID, nombre, tam, puntero);
}

void loggear_dialfs_escribir_archivo(int PID, char *nombre, int tam, int puntero)
{
    log_info(logger_obligatorio, "PID: <%d> - Escribir Archivo: <%s> - Tamaño a Escribir: <%d> - Puntero Archivo: <%d>", PID, nombre, tam, puntero);
}

void loggear_dialfs_inicio_compactacion(int PID)
{
    log_info(logger_obligatorio, "PID: <%d> - Inicio Compactación.", PID);
}

void loggear_dialfs_fin_compactacion(int PID)
{
    log_info(logger_obligatorio, "PID: <%d> - Fin Compactación.", PID);
}