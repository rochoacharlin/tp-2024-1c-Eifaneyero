#include "configuraciones.h"
#include "consola/consola.h"
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion/comunicacion.h>
#include "conexiones/conexiones.h"

t_log *logger_obligatorio;
t_log *logger_propio;
t_config *config;
int conexion_kernel_cpu_dispatch;
int conexion_kernel_cpu_interrupt;
pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;

int main(int argc, char *argv[])
{
    logger_obligatorio = crear_logger("kernel_obligatorio");
    logger_propio = crear_logger("kernel_propio");
    log_info(logger_propio, "Iniciando Kernel ...");

    config = iniciar_config(logger_propio, "kernel.config");

    // servidor();
    // conexion_kernel_cpu_dispatch = conexion_dispatch_con_CPU();
    // conexion_kernel_cpu_interrupt = conexion_interrupt_con_CPU();
    // iniciar_conexion_memoria();

    inicializar_listas_planificacion();
    inicializar_semaforos_planificacion();

    if (pthread_create(&hilo_planificador_largo_plazo, NULL, (void *)planificar_a_largo_plazo, NULL))
        log_error(logger_propio, "Error creando el hilo del planificador de largo plazo");
    if (pthread_create(&hilo_planificador_corto_plazo, NULL, (void *)planificar_a_corto_plazo_segun_algoritmo, NULL))
        log_error(logger_propio, "Error creando el hilo del planificador de corto plazo");

    consola_interactiva();

    // creo que falta terminar las conexiones
    log_destroy(logger_obligatorio);
    log_destroy(logger_propio);
    config_destroy(config);

    return 0;
}
