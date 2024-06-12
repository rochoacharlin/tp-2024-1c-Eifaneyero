#include <conexiones/conexiones.h>

int server_fd;

void iniciar_servidor_memoria(void)
{
    server_fd = iniciar_servidor(logger_propio, obtener_puerto_escucha());
    log_info(logger_propio, "Memoria lista para recibir clientes");
}

void iniciar_conexiones()
{
    iniciar_servidor_memoria();

    while (1)
    {
        int conexion_entrante = esperar_cliente(logger_propio, server_fd);
        int codigo_de_operacion = recibir_operacion(conexion_entrante);

        switch (codigo_de_operacion)
        {
        case CONEXION_IO:
            pthread_t hilo_io;
            pthread_create(&hilo_io, NULL, (void *)atender_io, &conexion_entrante);
            pthread_join(hilo_io, NULL);
            break;

        case CONEXION_KERNEL:
            pthread_t hilo_kernel;
            pthread_create(&hilo_kernel, NULL, (void *)atender_kernel, &conexion_entrante);
            pthread_detach(hilo_kernel);
            break;

        case CONEXION_CPU:

            pthread_t hilo_cpu;
            pthread_create(&hilo_cpu, NULL, (void *)atender_cpu, &conexion_entrante);
            pthread_detach(hilo_cpu);
            break;

        default:
            log_info(logger_propio, "Cliente no reconocido en memoria");
            break;
        }
    }
}