#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>

int main(int argc, char *argv[])
{
    char *modulo = "entradasalida";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Interfaz de I/O ...");

    t_config *config = iniciar_config(logger, "entradasalida.config");

    return 0;
}
