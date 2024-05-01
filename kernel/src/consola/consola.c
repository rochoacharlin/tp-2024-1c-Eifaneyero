#include "consola.h"

void consola_interactiva(void)
{
    char *leido;
    char *path;
    int PID;
    int valor;

    leido = readline("> ");
    while (1)
    {
        if (string_starts_with(leido, "EJECUTAR_SCRIPT"))
        {
            path = string_substring_from(leido, strlen("EJECUTAR_SCRIPT") + 1);
            // TODO
        }
        else if (string_starts_with(leido, "INICIAR_PROCESO"))
        {
            path = string_substring_from(leido, strlen("INICIAR_PROCESO") + 1);
            // TODO
        }
        else if (string_starts_with(leido, "FINALIZAR_SCRIPT"))
        {
            PID = atoi(string_substring_from(leido, strlen("FINALIZAR_SCRIPT") + 1));
            // TODO
        }
        else if (string_starts_with(leido, "DETENER_PLANIFICACION"))
        {
            // TODO
        }
        else if (string_starts_with(leido, "INICIAR_PLANIFICACION"))
        {
            // TODO
        }
        else if (string_starts_with(leido, "MULTIPROGRAMACION"))
        {
            valor = atoi(string_substring_from(leido, strlen("MULTIPROGRAMACION") + 1));
            // TODO
        }
        else if (string_starts_with(leido, "PROCESO_ESTADO"))
        {
            // TODO
        }
        else
        {
            log_error(logger, "Comando invalido.");
        }

        free(leido);
        leido = readline("> ");
    }

    free(leido);
}
