#include "conexiones.h"

int server_fd;
int sockets[3];

void atender_cpu()
{
    while (1)
    {
        t_list *valores_paquete;

        op_code codigo_operacion = recibir_operacion(sockets[0]);

        log_info(logger_propio, "se recibio un paquete");

        switch (codigo_operacion)
        {
        case (SOLICITUD_INSTRUCCION):

            t_solicitud_de_instruccion *solicitud = crear_solicitud_de_instruccion();

            valores_paquete = recibir_paquete(sockets[0]);
            generar_solicitud_de_instruccion(solicitud, valores_paquete);

            int pc = solicitud->desplazamiento;
            t_instruccion_cadena *instruccion = leer_instruccion(pc);

            // enviar a cpu la instruccion

            t_paquete *paquete_instruccion = crear_paquete(INSTRUCCION);

            serializar_instruccion(instruccion, paquete_instruccion);

            enviar_paquete(paquete_instruccion, sockets[0]);

            break;

        default:

            log_info(logger_propio, "el codigo de operacion es incorrecto");

            break;
        }

        list_destroy(valores_paquete);

        usleep(obtener_retardo_respuesta());
    }
}

void iniciar_conexiones()
{

    // esperar Conexiones

    esperar_a("CPU", sockets, server_fd);
    log_info(logger_propio, "Se conectó la CPU como cliente!");

    // esperar Kernel

    usleep(1000 * 500);
    esperar_a("Kernel", (sockets + 1), server_fd);
    log_info(logger_propio, "Se conectó la Kenel como cliente!");

    // eperar E/S

    usleep(1000 * 500);
    esperar_a("E/S", (sockets + 2), server_fd);
    log_info(logger_propio, "Se conectó la E/S como cliente!");
}

void iniciar_servidor_memoria()
{
    server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha());
    log_info(logger_propio, "Memoria lista para recibir clientes");
}