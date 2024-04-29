#include "main.h"

t_log *logger;
t_config *config;
int32_t procesos_creados = 0; // provisional

int main(int argc, char *argv[])
{
    char *modulo = "kernel";
    logger = crear_logger(modulo);
    log_info(logger, "Iniciando Kernel ...");

    config = iniciar_config(logger, "kernel.config");

    int conexion;
    // Provisorio
    int modo_ejecucion;
    printf("Elija su modo de ejecución (numerico) \n - 0 (Servidor) \n - 1 (Conectar a CPU) \n - 2 (Conectar a Memoria) \n");
    scanf("%d", &modo_ejecucion);
    switch (modo_ejecucion)
    {
    case SERVIDOR:
        int server_fd = iniciar_servidor(logger, obtener_puerto_escucha());
        log_info(logger, "Kernel listo para recibir clientes");
        int cliente_fd = esperar_cliente(logger, server_fd);
        log_info(logger, "Se conectó un cliente!");

        int32_t handshake_esperado = 4;
        int handshake_respuesta = handshake_servidor(logger, cliente_fd, handshake_esperado);

        close(cliente_fd); // TODO es provisional
        terminar_programa(server_fd, logger, config);
        break;

    case CONEXION_CPU:
    {
        int puerto;
        printf("Elija el puerto de escucha (numérico) \n - 0 (Dispatch) \n - 1 (Interrupt) \n ");
        scanf("%d", &puerto);

        if (puerto == 0)
        {
            conexion = crear_conexion(logger, obtener_ip_cpu(), obtener_puerto_cpu_dispatch());
            int32_t handshake = 5;
            int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
        }
        else if (puerto == 1)
        {
            conexion = crear_conexion(logger, obtener_ip_cpu(), obtener_puerto_cpu_interrupt());
            int32_t handshake = 6;
            int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
        }
        else
            log_error(logger, "Puerto de escucha invalido");

        terminar_programa(conexion, logger, config);
    }
    break;

    case CONEXION_MEMORIA:
    {
        conexion = crear_conexion(logger, obtener_ip_memoria(), obtener_puerto_memoria());
        int32_t handshake = 1;
        int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
        terminar_programa(conexion, logger, config);
        break;
    }

    default:
        log_error(logger, "Modo de ejecucion invalido");
        log_destroy(logger);
        config_destroy(config);
        break;
    }

    return 0;
}
