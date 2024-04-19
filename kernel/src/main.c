#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main(int argc, char *argv[])
{
    char *modulo = "kernel";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Kernel ...");

    t_config *config = iniciar_config(logger, "kernel.config");

    int conexion;
    // Provisorio
    int modo_ejecucion;
    printf("Elija su modo de ejecución (numerico) \n - 0 (Servidor) \n - 1 (Cliente-CPU) \n - 2 (Cliente-Memoria)\n");
    scanf("%d", &modo_ejecucion);
    switch (modo_ejecucion)
    {
    case SERVIDOR:
        int server_fd = iniciar_servidor(config, logger, "PUERTO_ESCUCHA");
        log_info(logger, "Kernel listo para recibir clientes");
        int cliente_fd = esperar_cliente(logger, server_fd);
        log_info(logger, "Se conectó un cliente!");

        close(cliente_fd); // TODO es provisional
        terminar_programa(server_fd, logger, config);
        break;

    case C_CPU:
        // Esta bien algo así por ahora? Es necesario? No sé como se definirán los puertos en el futuro. Idem en CPU
        int puerto = 2;
        while (puerto != 0 && puerto != 1)
        {
            printf("Elija el puerto de escucha (numérico) \n - 0 Dispatch \n - 1 Interrupt \n ");
            scanf("%d", &puerto);
        }
        if (puerto == 0)
            conexion = conectar_a(config, logger, "IP_CPU", "PUERTO_CPU_DISPATCH");
        else
            conexion = conectar_a(config, logger, "IP_CPU", "PUERTO_CPU_INTERRUPT");

        terminar_programa(conexion, logger, config);
        break;

    case C_MEMORIA:
        conexion = conectar_a(config, logger, "IP_MEMORIA", "PUERTO_MEMORIA");
        terminar_programa(conexion, logger, config);
        break;

    default:
        break;
    }

    return 0;
}
