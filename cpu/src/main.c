#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main(int argc, char *argv[])
{
    char *modulo = "cpu";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando CPU ...");

    t_config *config = iniciar_config(logger, "cpu.config");

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
            server_fd = iniciar_servidor(config, logger, "PUERTO_ESCUCHA_DISPATCH");
        else if (puerto == 1)
            server_fd = iniciar_servidor(config, logger, "PUERTO_ESCUCHA_INTERRUPT");
        else
            log_error(logger, "Puerto de escucha invalido");

        log_info(logger, "CPU lista para recibir clientes");
        int cliente_fd = esperar_cliente(logger, server_fd);
        log_info(logger, "Se conectó un cliente!");

        if (puerto == 0)
        {
            int32_t handshake_esperado = 5;
            int handshake_respuesta = handshake_servidor(logger, cliente_fd, handshake_esperado);
        }
        else if (puerto == 1)
        {
            int32_t handshake_esperado = 6;
            int handshake_respuesta = handshake_servidor(logger, cliente_fd, handshake_esperado);
        }

        close(cliente_fd); // TODO es provisional
        terminar_programa(server_fd, logger, config);
        break;

    case CONEXION_MEMORIA:
        int conexion = conectar_a(config, logger, "IP_MEMORIA", "PUERTO_MEMORIA");
        int32_t handshake = 1;
        int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
        terminar_programa(conexion, logger, config);
        break;

    default:
        log_error(logger, "Modo de ejecucion invalido");
        log_destroy(logger);
        config_destroy(config);
        break;
    }

    return 0;
}
