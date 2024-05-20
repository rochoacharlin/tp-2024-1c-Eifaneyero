#include "configuraciones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

int main(int argc, char *argv[])
{
    int conexion;
    logger_obligatorio = crear_logger("entradasalida_obligatorio");
    logger_propio = crear_logger("entradasalida_propio");
    log_info(logger_propio, "Iniciando Interfaz de I/O ...");

    config = iniciar_config(logger_propio, "entradasalida.config");

    int modo_ejecucion;
    printf("Elija su modo de ejecución (numerico) \n - 2 (Conectar a Memoria) \n - 3 (Conectar a Kernel) \n");
    scanf("%d", &modo_ejecucion);
    switch (modo_ejecucion)
    {
    case CONEXION_KERNEL:
    {
        conexion = crear_conexion(logger_propio, obtener_ip_kernel(), obtener_puerto_kernel());
        int32_t handshake = 4;
        int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
        terminar_programa(conexion, logger_propio, config);
    };
    break;

    case CONEXION_MEMORIA:
    {
        conexion = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
        int32_t handshake = 1;
        int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
        terminar_programa(conexion, logger_propio, config);
    };
    break;

    default:
        log_error(logger_propio, "Modo de ejecucion invalido");
        log_destroy(logger_propio);
        config_destroy(config);
        break;
    }

    return 0;
}
