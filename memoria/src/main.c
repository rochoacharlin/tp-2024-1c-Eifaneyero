
#include "configuraciones.h"
#include <utils/serializacion.h>
#include <utils/estructuras_compartidas.h>
#include "conexiones/conexiones.h"
#include "lectura/lectura.h"

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

    // Por cada Hilo

    t_list *valores_paquete;

    op_code codigo_operacion = recibir_operacion(cliente_fd);

    log_info(logger_propio, "se recibio un paquete");

    printf("el codigo operacion es: %d \n", codigo_operacion);

    switch (codigo_operacion)
    {
    case (SOLICITUD_INSTRUCCION):

        t_solicitud_de_instruccion *solicitud = crear_solicitud_de_instruccion();

        valores_paquete = recibir_paquete(cliente_fd);

        void generar_solicitud_de_instruccion(solicitud, valores_solicitud);

        int pc = solicitud->desplazamiento;

        log_info(logger_propio, "se recibio una solicitud desde cpu \n el valor del PC es:", pc);

        t_instruccion instruccion = leerInstruccion(solicitud->desplazamiento);

        // enviar a cpu la instruccion

        break;

    default:
        break;
    }

    list_destroy(valores_paquete);

    close(cliente_fd); // TODO es provisional
    terminar_programa(server_fd, logger_propio, config);
    log_destroy(logger_obligatorio);

    return 0;
}

// alternativa a serializar
/*
t_paquete *paquete_recibir(int socket)
{
    int size;
    void *buffer;
    t_list *valores = list_create();
    int tamanio;

    buffer = recibir_buffer(socket, &size);
    return paquete_deserializar(buffer, size);
}

t_paquete *paquete_deserializar(void *buffer, int size)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    int desplazamiento = 0;

    memcpy(&(paquete->codigo_operacion), buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    paquete->buffer = malloc(sizeof(t_buffer));

    memcpy(&(paquete->buffer->size), buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(paquete->buffer->stream, buffer + desplazamiento, size);
    desplazamiento += sizeof(int);

    return paquete;
}
*/