#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>

int main()
{
    char *modulo = "kernel";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando Kernel ...");

    t_config *config = iniciar_config(logger, "kernel.config");

    return 0;
}
