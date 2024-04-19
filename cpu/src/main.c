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

    printf("Elija su modo de ejecución (numerico) \n - 0 (Servidor-Kernel) \n - 2 (Cliente-Memoria)\n");
    scanf("%d", &modo_ejecucion);
    switch (modo_ejecucion)
    {
    case SERVIDOR:
        int puerto = -1;
        int server_fd;
        while (puerto != 0 && puerto != 1)
        {
            printf("Elija el puerto de escucha (numérico) \n - 0 Dispatch \n - 1 Interrupt \n ");
            scanf("%d", &puerto);
        }
        if (puerto == 0)
            server_fd = iniciar_servidor(config, logger, "PUERTO_ESCUCHA_DISPATCH");
        else
            server_fd = iniciar_servidor(config, logger, "PUERTO_ESCUCHA_INTERRUPT");

        log_info(logger, "CPU lista para recibir clientes");
        int cliente_fd = esperar_cliente(logger, server_fd);
        log_info(logger, "Se conectó un cliente!");
        close(cliente_fd); // TODO es provisional
        terminar_programa(server_fd, logger, config);
        break;

    case C_MEMORIA:
        int conexion = conectar_a(config, logger, "IP_MEMORIA", "PUERTO_MEMORIA");
        terminar_programa(conexion, logger, config);
        break;

    default:
        break;
    }

    return 0;
}
