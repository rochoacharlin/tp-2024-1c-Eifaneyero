#include "configuraciones.h"
#include <utils/estructuras_compartidas/estructuras_compartidas.h>
#include "interface_cpu.h"
#include "conexiones/conexiones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;
t_dictionary *registros_cpu;

int cpu_funcionando = 1; // En caso de querer terminar cpu.

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("cpu_obligatorio");
    logger_propio = crear_logger("cpu_propio");
    log_info(logger_propio, "Iniciando CPU ...");
    config = iniciar_config(logger_propio, "cpu.config");

    // registros_cpu = crear_registros_cpu();

    iniciar_conexiones();

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