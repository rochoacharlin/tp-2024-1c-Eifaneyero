#include "conexiones.h"

void servidor(void)
{
    int server_fd = iniciar_servidor(logger, obtener_puerto_escucha());
    log_info(logger, "Kernel listo para recibir clientes");
    int cliente_fd = esperar_cliente(logger, server_fd);
    log_info(logger, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    int handshake_respuesta = handshake_servidor(logger, cliente_fd, handshake_esperado);
}

void conexion_interrupt_con_CPU(void)
{
    int conexion = crear_conexion(logger, obtener_ip_cpu(), obtener_puerto_cpu_interrupt());
    int32_t handshake = 5;
    int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
}

void conexion_dispatch_con_CPU(void)
{
    int conexion = crear_conexion(logger, obtener_ip_cpu(), obtener_puerto_cpu_dispatch());
    int32_t handshake = 5;
    int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
}

void conexion_con_memoria(void)
{
    int conexion = crear_conexion(logger, obtener_ip_memoria(), obtener_puerto_memoria());
    int32_t handshake = 1;
    int handshake_respuesta = handshake_cliente(logger, conexion, handshake);
}