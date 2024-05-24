#include "conexiones.h"

// socket
int conexion_cpu_memoria;

void servidor_dispatch(void)
{
    int server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha_dispatch());
    log_info(logger_propio, "CPU lista para recibir clientes (dispatch)");
    int cliente_fd = esperar_cliente(logger_propio, server_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    int handshake_respuesta = handshake_servidor(logger_propio, cliente_fd, handshake_esperado);
}

void servidor_interrupt(void)
{
    int server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha_interrupt());
    log_info(logger_propio, "CPU lista para recibir clientes (interrupt)");
    int cliente_fd = esperar_cliente(logger_propio, server_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    int handshake_respuesta = handshake_servidor(logger_propio, cliente_fd, handshake_esperado);
}

int conexion_con_memoria(void)
{
    int conexion = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
    int32_t handshake = 1;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
    return conexion;
}
