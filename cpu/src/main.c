#include "configuraciones.h"
#include "../../utils/src/utils/estructuras_compartidas.h"
#include "interface_cpu.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;
t_dictionary *registros_cpu;

void set(char *nombre_registro, void *valor);
void sum(char *nombre_destino, char *nombre_origen);
void sub(char *nombre_destino, char *nombre_origen);
void jnz(char *nombre_registro, void *nro_instruccion);
void io_gen_sleep(char *nombre_interfaz, void *unidades_de_trabajo);

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("cpu_obligatorio");
    logger_propio = crear_logger("cpu_propio");
    log_info(logger_propio, "Iniciando CPU ...");

    config = iniciar_config(logger_propio, "cpu.config");

    registros_cpu = crear_registros_cpu();

    int modo_ejecucion;
    printf("Elija su modo de ejecución (numerico) \n - 0 (Servidor) \n - 2 (Conectar a Memoria) \n");
    scanf("%d", &modo_ejecucion);
    switch (modo_ejecucion)
    {
    case SERVIDOR:
        int puerto;
        int server_fd;
        printf("Elija el puerto de escucha (numérico) \n - 0 (Dispatch) \n - 1 (Interrupt) \n ");
        scanf("%d", &puerto);

        if (puerto == 0)
            server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha_dispatch());
        else if (puerto == 1)
            server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha_interrupt());
        else
            log_error(logger_propio, "Puerto de escucha invalido");

        log_info(logger_propio, "CPU lista para recibir clientes");
        int cliente_fd = esperar_cliente(logger_propio, server_fd);
        log_info(logger_propio, "Se conectó un cliente!");

        if (puerto == 0)
        {
            int32_t handshake_esperado = 5;
            int handshake_respuesta = handshake_servidor(logger_propio, cliente_fd, handshake_esperado);
        }
        else if (puerto == 1)
        {
            int32_t handshake_esperado = 6;
            int handshake_respuesta = handshake_servidor(logger_propio, cliente_fd, handshake_esperado);
        }

        close(cliente_fd); // TODO es provisional
        terminar_programa(server_fd, logger_propio, config);
        break;

    case CONEXION_MEMORIA:

        int conexion = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
        int32_t handshake = 1;
        int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);

        // en una futura implementacion utilizaremos la MMU:  solicitar_instrucion(pid, nro_pagina, desplazamiento);
        solicitar_lectura_de_instruccion(conexion, dictionary_get(registros_cpu, "PC"));
        // provisoriamente lo hacemos con readline: solicitar _instrucion(conexion,desplazamiento)

        log_info(logger_propio, "CPU Solicita Lectura de Instruccion a Memoria");

        terminar_programa(conexion, logger_propio, config);
        break;

    default:
        log_error(logger_propio, "Modo de ejecucion invalido");
        log_destroy(logger_propio);
        config_destroy(config);
        break;
    }

    return 0;
}

void set(char *nombre_registro, void *valor)
{
    if (strlen(nombre_registro) == 3 || !strcmp(nombre_registro, "SI") || !strcmp(nombre_registro, "DI") || !strcmp(nombre_registro, "PC")) // caso registros de 4 bytes
    {
        uint32_t *registro = dictionary_get(registros_cpu, nombre_registro);
        uint32_t *val = (uint32_t *)valor;
        *registro = *val;
        free(val);
    }
    else if (strlen(nombre_registro) == 2) // caso registros de 1 bytes
    {
        uint8_t *registro = dictionary_get(registros_cpu, nombre_registro);
        uint8_t *val = (uint8_t *)valor;
        *registro = *val;
        free(val);
    }
}

void sum(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(registros_cpu, nombre_origen);
    val_destino += val_origen;
    set(nombre_destino, &val_destino);
}

void sub(char *nombre_destino, char *nombre_origen)
{
    uint32_t val_destino = obtener_valor_registro(registros_cpu, nombre_destino);
    uint32_t val_origen = obtener_valor_registro(registros_cpu, nombre_origen);
    val_destino -= val_origen;
    set(nombre_destino, &val_destino);
}

void jnz(char *nombre_registro, void *nro_instruccion)
{
    uint32_t val = obtener_valor_registro(registros_cpu, nombre_registro);
    if (val != 0)
    {
        set("PC", nro_instruccion);
    }
}

void io_gen_sleep(char *nombre_interfaz, void *unidades_de_trabajo)
{
    // TODO falta que la CPU le pueda enviar un proceso al Kernel
}