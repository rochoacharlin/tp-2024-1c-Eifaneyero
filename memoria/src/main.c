
#include "configuraciones.h"
#include <utils/serializacion.h>
#include <utils/estructuras_compartidas.h>
#include "conexiones/conexiones.h"
#include "lectura/lectura.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

int server_fd;
int sockets[3];

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("memoria_obligatorio");
    logger_propio = crear_logger("memoria_propio");
    log_info(logger_propio, "Iniciando Memoria ...");

    config = iniciar_config(logger_propio, "memoria.config");

    // inicio servidor y conexiones

    iniciar_servidor_memoria();

    iniciar_conexiones();

    // hilos a futuro

    atender_cpu();

    terminar_programa(server_fd, logger_propio, config);
    free(sockets);
    log_destroy(logger_obligatorio);

    return 0;
}
