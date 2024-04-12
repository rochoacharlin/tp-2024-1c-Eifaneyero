#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>

int main(int argc, char *argv[])
{
    char *modulo = "cpu";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando CPU ...");

    t_config *config = iniciar_config(logger, "cpu.config");

    return 0;
}
