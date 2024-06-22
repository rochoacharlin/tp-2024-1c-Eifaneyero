#include "consola/consola.h"
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include <utils/estructuras_compartidas/registros_cpu.h>
#include "conexiones/conexiones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;

pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;
pthread_t hilo_servidor;
pthread_t hilo_recursos_liberados;

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("kernel_obligatorio");
    logger_propio = crear_logger("kernel_propio");
    log_info(logger_propio, "Iniciando Kernel ...");

    config = iniciar_config(logger_propio, "kernel.config");

    if (pthread_create(&hilo_servidor, NULL, (void *)servidor, NULL))
        log_error(logger_propio, "Error creando el hilo servidor para recibir las interfaces.");
    conexion_dispatch_con_CPU();
    conexion_interrupt_con_CPU();
    conexion_memoria();

    inicializar_listas_planificacion();
    inicializar_semaforos_planificacion();

    if (pthread_create(&hilo_recursos_liberados, NULL, (void *)manejar_recursos_liberados, NULL))
        log_error(logger_propio, "Error creando el hilo de recursos liberados.");

    if (pthread_create(&hilo_planificador_largo_plazo, NULL, (void *)planificar_a_largo_plazo, NULL))
        log_error(logger_propio, "Error creando el hilo del planificador de largo plazo.");
    if (pthread_create(&hilo_planificador_corto_plazo, NULL, (void *)planificar_a_corto_plazo_segun_algoritmo, NULL))
        log_error(logger_propio, "Error creando el hilo del planificador de corto plazo.");

    consola_interactiva();

    destruir_semaforos_planificacion();
    destruir_listas_planificacion();

    close(conexion_kernel_cpu_dispatch);
    close(conexion_kernel_cpu_interrupt);
    close(conexion_kernel_memoria);
    log_destroy(logger_obligatorio);
    log_destroy(logger_propio);
    config_destroy(config);

    return 0;
}