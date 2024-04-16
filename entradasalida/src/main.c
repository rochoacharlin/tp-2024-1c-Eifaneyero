#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main(int argc, char *argv[])
{

    int conexion_kernel, conexion_memoria;
    char *modulo = "entradasalida";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Interfaz de I/O ...");

    t_config *config = iniciar_config(logger, "entradasalida.config");

    // conecta al kernel
    conexion_kernel = conectar_a(config, logger, "IP_KERNEL", "PUERTO_KERNEL");

    // conecta a la memoria
    conexion_memoria = conectar_a(config, logger, "IP_MEMORIA", "PUERTO_MEMORIA");

    // terminamos el programa cerrando las conexiones
    close(conexion_memoria);
    terminar_programa(conexion_kernel, logger, config);

    return 0;
}
