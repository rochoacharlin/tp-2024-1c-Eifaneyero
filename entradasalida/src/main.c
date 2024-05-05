#include "configuraciones.h"

t_log *logger;
t_config *config;

int main(int argc, char *argv[])
{
    int conexion;
    char *modulo = "entradasalida";
    logger = crear_logger(modulo);
    log_info(logger, "Iniciando Interfaz de I/O ...");

    config = iniciar_config(logger, "entradasalida.config");

    int modo_ejecucion;
    printf("Elija su modo de ejecución (numerico) \n - 2 (Conectar a Memoria) \n - 3 (Conectar a Kernel) \n");
    scanf("%d", &modo_ejecucion);
    switch (modo_ejecucion)
    {
    case CONEXION_KERNEL:
    {
        conexion = crear_conexion(logger, obtener_ip_kernel(), obtener_puerto_kernel());
        int32_t handshake = 4;
        int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
        terminar_programa(conexion, logger, config);
    };
    break;

    case CONEXION_MEMORIA:
    {
        conexion = crear_conexion(logger, obtener_ip_memoria(), obtener_puerto_memoria());
        int32_t handshake = 1;
        int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
        terminar_programa(conexion, logger, config);
    };
    break;

    default:
        log_error(logger, "Modo de ejecucion invalido");
        log_destroy(logger);
        config_destroy(config);
        break;
    }

    return 0;
}
