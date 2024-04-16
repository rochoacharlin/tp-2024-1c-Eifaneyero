#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main()
{
    char *modulo = "kernel";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Kernel ...");

    t_config *config = iniciar_config(logger, "kernel.config");
    int conexion;
    // Provisorio
    int z;
    printf("Elija su modo de ejecución (numerico) \n - 1 (Servidor) \n - 2 (Cliente-Memoria)\n");
    scanf("%d", &z);
    switch (z)
    {
    case SERVIDOR:

        int server_fd = iniciar_servidor(config, logger, "PUERTO_ESCUCHA");
        log_info(logger, "Kernel listo para recibir clientes");
        int cliente_fd = esperar_cliente(logger, server_fd);
        log_info(logger, "Se conectó un cliente!");

        close(cliente_fd); // TODO es provisional
        terminar_programa(server_fd, logger, config);
        break;

    case C_MEMORIA:

        conexion = conectar_a(config, logger, "IP_MEMORIA", "PUERTO_MEMORIA");
        terminar_programa(conexion, logger, config);

        break;

    case C_CPU:
        // TODO
        break;

    default:
        break;
    }

    return 0;
}
