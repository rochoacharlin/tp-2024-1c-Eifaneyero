#include <utils/funcionalidades_basicas.h>

t_log *crear_logger(char *modulo) // Podria ampliarse o cambiarse
{
    char *nombre_logger = string_duplicate(modulo);
    char *log = ".log";
    string_append_with_format(&nombre_logger, "%s", log);

    t_log *nuevo_logger = log_create(nombre_logger, modulo, 1, LOG_LEVEL_INFO);

    if (nuevo_logger == NULL)
    {
        printf("No se pudo iniciar un logger\n");
        exit(EXIT_FAILURE);
    }

    return nuevo_logger;
}

t_config *iniciar_config(t_log *logger, char *ruta_archivo)
{
    t_config *nuevo_config = config_create(ruta_archivo);
    if (nuevo_config == NULL)
    {
        log_error(logger, "No se pudo crear la configuracion en %s", ruta_archivo);
        exit(EXIT_FAILURE);
    }
    return nuevo_config;
}

void terminar_programa(int conexion, t_log *logger, t_config *config)
{
    close(conexion);
    log_destroy(logger);
    config_destroy(config);
}