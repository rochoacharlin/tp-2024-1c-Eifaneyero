#include "configuraciones.h"

void loggear_fetch_instrucccion(uint32_t PID, uint32_t PC)
{
    log_info(logger_obligatorio, "Fetch Instrucción: PID: <%d> - FETCH - Program Counter: <%d>", PID, PC);
}
/*
//TODO: Varia cantidad de parametros. (Me la complica) Mas fácil log info para cada instruccion.
void loggear_instrucion_ejecutada(uint32_t PID, t_instruccion instruccion)
{
    log_info(logger_obligatorio, "Instrucción Ejecutada: PID: <%d> - Ejecutando: <%s> - <PARAMETROS>", PID, instruccion->id); /
}
*/
void loggear_tlb_hit(uint32_t PID, int numero_pagina)
{
    // TODO:
}

void loggear_tlb_miss(uint32_t PID, int numero_pagina)
{
    // TODO
}

void loggear_obtener_marco(uint32_t PID, int numero_pagina, int numero_marco)
{
    // TODO
}

void loggear_lectura_o_escritura_memoria(uint32_t PID, char accion, int direccion_fisica, int valor) // ni idea si estos son los tipos de datos correctos
{
    // TODO
}