#include <utils/hello.h>

void decir_hola(char *quien)
{
    printf("Hola desde %s!!\n", quien);
}

t_log *crear_logger(char *modulo) // Podria ampliarse o cambiarse
{

    char *name_logger = string_duplicate(modulo);
    char *log = ".log";
    string_append_with_format(&name_logger, "%s", log);

    t_log *new_logger;
    new_logger = log_create(name_logger, modulo, 1, LOG_LEVEL_INFO);

    if (new_logger == NULL)
    {
        printf("No se pudo iniciar un logger\n");

        exit(1);
    }

    return new_logger;
}