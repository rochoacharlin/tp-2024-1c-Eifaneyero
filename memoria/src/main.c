
#include "configuraciones.h"
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/contexto_ejecucion.h>
#include "conexiones/conexiones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("memoria_obligatorio");
    logger_propio = crear_logger("memoria_propio");
    log_info(logger_propio, "Iniciando Memoria ...");

    config = iniciar_config(logger_propio, "memoria.config");

    // inicializar_memoria();
    // iniciar_conexiones();

    int cantidad_marcos = (obtener_tam_memoria() + obtener_tam_pagina() - 1) / obtener_tam_pagina();
    size_t bytes_necesarios = (cantidad_marcos + 8 - 1) / 8;
    void *espacio_bitarray = malloc(bytes_necesarios);
    memset(espacio_bitarray, 1, bytes_necesarios);
    t_bitarray *bitarray = bitarray_create_with_mode(espacio_bitarray, bytes_necesarios, LSB_FIRST);
    log_info(logger_propio, "test:%d", bitarray_test_bit(bitarray, 0));
    bitarray_set_bit(bitarray, 0);
    log_info(logger_propio, "test:%d", bitarray_test_bit(bitarray, 0));
    bitarray_clean_bit(bitarray, 1);
    log_info(logger_propio, "test:%d", bitarray_test_bit(bitarray, 0));

    // terminar_programa(server_fd, logger_propio, config);
    //  free(sockets);
    // log_destroy(logger_obligatorio);

    return 0;
}
