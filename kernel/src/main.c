#include "configuraciones.h"
#include "consola/consola.h"
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>
#include "conexiones/conexiones.h"

t_log *logger;
t_config *config;
pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;

int main(int argc, char *argv[])
{
    char *modulo = "kernel";
    logger = crear_logger(modulo);
    log_info(logger, "Iniciando Kernel ...");

    config = iniciar_config(logger, "kernel.config");

    inicializar_listas_planificacion();
    inicializar_semaforos_planificacion();

    // servidor();
    // conexion_con_CPU();
    // conexion_con_memoria();

    if (pthread_create(&hilo_planificador_largo_plazo, NULL, (void *)planificar_a_largo_plazo, NULL))
        log_error(logger, "Error creando el hilo del planificador de largo plazo");
    if (pthread_create(&hilo_planificador_corto_plazo, NULL, (void *)planificar_a_corto_plazo_segun_algoritmo, NULL))
        log_error(logger, "Error creando el hilo del planificador de corto plazo");

    consola_interactiva();

    return 0;
}
