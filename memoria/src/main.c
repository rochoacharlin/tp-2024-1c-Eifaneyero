
#include "configuraciones.h"
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/contexto_ejecucion.h>
#include "conexiones/conexiones.h"
#include "lectura/lectura.h"
#include "gestion_memoria.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("memoria_obligatorio");
    logger_propio = crear_logger("memoria_propio");
    log_info(logger_propio, "Iniciando Memoria ...");

    config = iniciar_config(logger_propio, "memoria.config");

    inicializar_memoria();
    iniciar_conexiones();

    // terminar_programa(server_fd, logger_propio, config);
    //  free(sockets);
    // log_destroy(logger_obligatorio);

    return 0;
}
