#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>

int main(int argc, char *argv[])
{
    char *modulo = "memoria";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Memoria ...");

    t_config *config = iniciar_config(logger, "memoria.config");

    return 0;
}
