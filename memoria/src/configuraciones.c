#include "configuraciones.h"

void loggear_creacion_destruccion_tabla_de_paginas(uint32_t PID, int cantidad_paginas)
{
    // TODO
}

void loggear_acceso_tablas_de_paginas(uint32_t PID, int pagina, int marco)
{
    log_info(logger_obligatorio, "PID: <%d> - Pagina: <%d> - Marco: <%d>", PID, pagina, marco);
}

void loggear_ampliacion_de_proceso(uint32_t PID, int tamanio_actual, int tamanio_a_ampliar)
{
    log_info(logger_obligatorio, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Ampliar: <%d>", PID, tamanio_actual, tamanio_a_ampliar);
}

void loggear_reduccion_de_proceso(uint32_t PID, int tamanio_actual, int tamanio_a_reducir)
{
    log_info(logger_obligatorio, "PID: <%d> - Tamaño Actual: <%d> - Tamaño a Reducir: <%d>", PID, tamanio_actual, tamanio_a_reducir);
}

void loggear_acceso_a_espacio_de_usuario(uint32_t PID, int accion, int direccion_fisica, int tamanio)
{
    // TODO
}
