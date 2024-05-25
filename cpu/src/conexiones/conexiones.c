#include "conexiones.h"

int conexion_cpu_memoria;
int conexion_cpu_kernel_dispatch;
int conexion_cpu_kernel_interrupt;

pthread_mutex_t *mutex_interrupt;

pthread_t th_dispatch;
pthread_t th_interrupt;

/*
el Contexto de Ejecución debe ser devuelto a través de la conexión de dispatch,
quedando la conexión de interrupt dedicada solamente a recibir mensajes de interrupción.
*/

void iniciar_servidor_dispatch(void)
{
    int server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha_dispatch());
    log_info(logger_propio, "CPU lista para recibir clientes (dispatch)");
    conexion_cpu_kernel_dispatch = esperar_cliente(logger_propio, server_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    int handshake_respuesta = handshake_servidor(logger_propio, conexion_cpu_kernel_dispatch, handshake_esperado);
}

void iniciar_servidor_interrupt(void)
{
    int server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha_interrupt());
    log_info(logger_propio, "CPU lista para recibir clientes (interrupt)");
    conexion_cpu_kernel_interrupt = esperar_cliente(logger_propio, server_fd);
    log_info(logger_propio, "Se conectó un cliente!");

    int32_t handshake_esperado = 4;
    int handshake_respuesta = handshake_servidor(logger_propio, conexion_cpu_kernel_interrupt, handshake_esperado);
}

void iniciar_conexion_memoria(void)
{
    conexion = crear_conexion(logger_propio, obtener_ip_memoria(), obtener_puerto_memoria());
    int32_t handshake = 1;
    int handshake_respuesta = handshake_cliente(logger_propio, conexion, handshake);
}

void iniciar_conexiones()
{
    iniciar_servidor_dispatch();
    iniciar_servidor_interrupt();
    iniciar_conexion_memoria();

    pthread_create(&th_dispatch, NULL, (void *)atender_dispatch, NULL);
    pthread_create(&th_interrupt, NULL, (void *)atender_interrupt, NULL);

    pthread_join(th_dispatch, NULL);
    pthread_join(th_interrupt, NULL);
}

void atender_dispatch()
{
    log_info(logger_propio, "CPU escuchando puerto dispatch");
    /*
    while (1)
    {
        t_contexto_ejecucion *contexto = recibir_contexto_de_kernel(conexion_cpu_kernel_dispatch);
        ciclo_de_instruccion(contexto, socket_kernel_dispatch);
        destruir_contexto();
    }
    */
}

void atender_interrupt()
{
    log_info(cpuLogger, "CPU escuchando puerto interrupt");
    /*
    while (1)
    {
        // TODO: interrupcion = recibir_interrupcion_kernel(conexion_cpu_kernel_interrupt);
        // Necesitaría recibir algo en particular? O solo quedar a la espera de mensajes de interrupción?
        hay_interrupcion = true;
    }
    */
}
