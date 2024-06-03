#include "configuraciones.h"
#include <utils/estructuras_compartidas/contexto_ejecucion.h>
#include "interface_cpu.h"
#include "conexiones/conexiones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;
t_dictionary *registros_cpu;
void chicken_test();

int cpu_funcionando = 1; // En caso de querer terminar cpu.

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("cpu_obligatorio");
    logger_propio = crear_logger("cpu_propio");
    log_info(logger_propio, "Iniciando CPU ...");
    config = iniciar_config(logger_propio, "cpu.config");

    // iniciar_conexiones();
    chicken_test();

    log_info(logger_propio, "Conexiones con kernel y Memoria exitosas");

    while (cpu_funcionando)
    {
        // :)
    }

    // en una futura implementacion utilizaremos la MMU:  solicitar_instrucion(pid, nro_pagina, desplazamiento);
    // solicitar_lectura_de_instruccion(conexion, dictionary_get(registros_cpu, "PC"));
    // provisoriamente lo hacemos con readline: solicitar _instrucion(conexion,desplazamiento)

    // log_info(logger_propio, "CPU Solicita Lectura de Instruccion a Memoria");

    // terminar_programa(conexion, logger_propio, config);

    log_destroy(logger_obligatorio);
    log_destroy(logger_propio);
    config_destroy(config);
    return 0;
}

void chicken_test()
{
    iniciar_servidor_dispatch();
    int opCode = recibir_operacion(conexion_cpu_kernel_dispatch);
    log_info(logger_propio, "opCode: %d", opCode);
    if (opCode == CONTEXTO_EJECUCION)
    {
        log_info(logger_propio, "Contexto recibido por CPU");
        t_contexto *contexto = recibir_contexto(conexion_cpu_kernel_dispatch);
        log_info(logger_propio, "PID: %d", contexto->PID);
        log_info(logger_propio, "PC: %d", obtener_valor_registro(contexto->registros_cpu, "PC"));
        log_info(logger_propio, "EBX: %d", obtener_valor_registro(contexto->registros_cpu, "EBX"));

        ciclo_de_instruccion(contexto);

        destruir_contexto(contexto);
    }
    else
    {
        log_info(logger_propio, "Something else happend");
    }
}