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
        else if (strcmp(leido, "DETENER_PLANIFICACION") == 0)
        {
            // TODO
        }
        else if (strcmp(leido, "INICIAR_PLANIFICACION") == 0)
        {
            // TODO
        }
        else if (string_starts_with(leido, "MULTIPROGRAMACION"))
        {
            valor = atoi(string_substring_from(leido, strlen("MULTIPROGRAMACION") + 1));
            // TODO
        }
        else if (strcmp(leido, "PROCESO_ESTADO") == 0)
        {
            listar_procesos_por_cada_estado();
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

void listar_procesos_por_cada_estado(void)
{
    // listar_procesos_por_estado("NEW", pcbs_en_NEW);
    listar_procesos_por_estado("READY", pcbs_en_READY);
    // listar_procesos_por_estado("EXEC", pcbs_en_EXEC);
    // listar_procesos_por_estado("BLOCKED", pcbs_en_BLOCKED);
    // listar_procesos_por_estado("EXIT", pcbs_en_EXIT);
}

void listar_procesos_por_estado(char *estado, t_list lista)
{
    printf("%s:", estado);
    while (list_iterator_has_next(lista))
    {
        t_pcb *pcb = list_iterator_next(lista);
        printf("    PID: %d", pcb->PID);
    }
}