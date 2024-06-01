#include "conexiones.h"

int conexion_kernel_cpu_dispatch;
int conexion_kernel_cpu_interrupt;
int conexion_kernel_memoria;
int servidor_kernel_fd;

void servidor(void)
{
    servidor_kernel_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha());
    log_info(logger_propio, "Kernel listo para recibir clientes");
    int cliente_fd = esperar_cliente(logger_propio, servidor_kernel_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    if (handshake_servidor(logger_propio, cliente_fd, handshake_esperado) == 0)
        log_error(logger_propio, "No se pudo conectar correctamente con el cliente");
}

void conexion_interrupt_con_CPU(void)
{
    conexion_kernel_cpu_interrupt = crear_conexion(logger_propio, obtener_ip_cpu(), obtener_puerto_cpu_interrupt());
    int32_t handshake = 5;
    if (handshake_cliente(logger_propio, conexion_kernel_cpu_interrupt, handshake) == 0)
        log_error(logger_propio, "No se pudo conectar correctamente con el servidor");
}

void conexion_dispatch_con_CPU(void)
{
    conexion_kernel_cpu_dispatch = crear_conexion(logger_propio, obtener_ip_cpu(), obtener_puerto_cpu_dispatch());
    int32_t handshake = 5;
    if (handshake_cliente(logger_propio, conexion_kernel_cpu_interrupt, handshake) == 0)
        log_error(logger_propio, "No se pudo conectar correctamente con el servidor");
}

void conexion_memoria(void)
{
    conexion_kernel_memoria = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
    int32_t handshake = 1;
    if (handshake_cliente(logger_propio, conexion_kernel_cpu_interrupt, handshake) == 0)
        log_error(logger_propio, "No se pudo conectar correctamente con el servidor");
}