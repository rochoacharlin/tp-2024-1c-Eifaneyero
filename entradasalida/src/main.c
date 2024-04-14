#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main(int argc, char *argv[])
{

    int conexion;
    char *modulo = "entradasalida";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Interfaz de I/O ...");

    t_config *config = iniciar_config(logger, "entradasalida.config");

    // conecta al kernel
    conexion = conectar_a(config, logger, "IP_KERNEL", "PUERTO_KERNEL");

    terminar_programa(conexion, logger, config);
    return 0;
}
