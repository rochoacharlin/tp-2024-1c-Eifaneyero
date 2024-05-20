#include "configuraciones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("memoria_obligatorio");
    logger_propio = crear_logger("memoria_propio");
    log_info(logger_propio, "Iniciando Memoria ...");

    config = iniciar_config(logger_propio, "memoria.config");

    int server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha());
    log_info(logger_propio, "Memoria lista para recibir clientes");
    int cliente_fd = esperar_cliente(logger_propio, server_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 1;
    int handshake_respuesta = handshake_servidor(logger_propio, cliente_fd, handshake_esperado);

    close(cliente_fd); // TODO es provisional
    terminar_programa(server_fd, logger_propio, config);
    log_destroy(logger_obligatorio);

    return 0;
}
