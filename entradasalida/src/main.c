#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main(int argc, char *argv[])
{
    int conexion;
    char *modulo = "entradasalida";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Interfaz de I/O ...");

    t_config *config = iniciar_config(logger, "entradasalida.config");

    int modo_ejecucion;
    printf("Elija su modo de ejecución (numerico) \n - 2 (Conectar a Memoria) \n - 3 (Conectar a Kernel) \n");
    scanf("%d", &modo_ejecucion);
    switch (modo_ejecucion)
    {
    case CONEXION_KERNEL:
        conexion = conectar_a(config, logger, "IP_KERNEL", "PUERTO_KERNEL");
        int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
        terminar_programa(conexion, logger, config);
        break;

    case CONEXION_MEMORIA:
        conexion = conectar_a(config, logger, "IP_MEMORIA", "PUERTO_MEMORIA");
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
