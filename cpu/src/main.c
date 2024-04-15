#include <stdlib.h>
#include <stdio.h>
#include <utils/funcionalidades_basicas.h>
#include <utils/comunicacion.h>

int main(int argc, char *argv[])
{
    char *modulo = "cpu";
    t_log *logger = crear_logger(modulo);
    log_info(logger, "Iniciando CPU ...");

    t_config *config = iniciar_config(logger, "cpu.config");

    // conecta con la memoria
    int conexion = conectar_a(config, logger, "IP_MEMORIA", "PUERTO_MEMORIA");

    terminar_programa(conexion, logger, config);

    return 0;
}
