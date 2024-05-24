#include "configuraciones.h"
#include "../../utils/src/utils/estructuras_compartidas.h"
#include "interface_cpu.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;
t_dictionary *registros_cpu;

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("cpu_obligatorio");
    logger_propio = crear_logger("cpu_propio");
    log_info(logger_propio, "Iniciando CPU ...");

    config = iniciar_config(logger_propio, "cpu.config");

    registros_cpu = crear_registros_cpu();

    // servidor_dispatch(); Queda a espera de kernel.
    // servidor_interrupt(); Queda a espera de kernel.
    // conexion_con_memoria();

    // Pase funciones de conexion a conexiones

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