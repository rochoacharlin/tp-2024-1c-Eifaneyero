#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main(int argc, char *argv[])
{
    char *modulo = "memoria";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Memoria ...");

    t_config *config = iniciar_config(logger, "memoria.config");

    int server_fd = iniciar_servidor(config, logger, "PUERTO_ESCUCHA");
    log_info(logger, "Memoria lista para recibir clientes");
    int cliente_fd = esperar_cliente(logger, server_fd);
    log_info(logger, "Se conectó un cliente!");

    close(cliente_fd); // TODO es provisional
    terminar_programa(server_fd, logger, config);

    return 0;
}
