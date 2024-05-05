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
            // TODO: Leer un archivo de comandos y correrlos.
        }
        else if (string_starts_with(leido, "INICIAR_PROCESO"))
        {
            path = string_substring_from(leido, strlen("INICIAR_PROCESO") + 1);
            // COMPLETAR: Falta toda la parte de la memoria y filesystem.
            t_pcb *pcb = crear_pcb();
            ingresar_pcb_a_NEW(pcb);
        }
        else if (string_starts_with(leido, "FINALIZAR_PROCESO"))
        {
            PID = atoi(string_substring_from(leido, strlen("FINALIZAR_SCRIPT") + 1));
            // TODO: Finalizar proceso, liberar recursos, archivos y memoria
        }
        else if (strcmp(leido, "DETENER_PLANIFICACION") == 0)
        {
            // COMPLETAR: El proceso que se encuentra en ejecución NO es desalojado,
            //            pero una vez que salga de EXEC se va a pausar el manejo de su motivo de desalojo.
            sem_wait(&planificacion_liberada);
            sem_post(&planificacion_pausada);
        }
        else if (strcmp(leido, "INICIAR_PLANIFICACION") == 0)
        {
            sem_wait(&planificacion_pausada); // para asegurar que libere la planificacion cuando este pausada.
            sem_post(&planificacion_liberada);
        }
        else if (string_starts_with(leido, "MULTIPROGRAMACION"))
        {
            valor = atoi(string_substring_from(leido, strlen("MULTIPROGRAMACION") + 1));
            // TODO: Cambiar el nivel de multiprogramacion
            // Lo cambiamos directamente en el archivo de configuracion o en una variable del codigo?
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
    destruir_semaforos_planificacion();
    destruir_listas_planificacion();
}

void listar_procesos_por_cada_estado(void)
{
    listar_procesos_por_estado("NEW", pcbs_en_NEW);
    listar_procesos_por_estado("READY", pcbs_en_READY);
    // listar_procesos_por_estado("EXEC", pcbs_en_EXEC);
    // listar_procesos_por_estado("BLOCKED", pcbs_en_BLOCKED);
    // listar_procesos_por_estado("EXIT", pcbs_en_EXIT);
}

void listar_procesos_por_estado(char *estado, t_list *lista)
{
    printf("%s: \n", estado);
    for (int i = 0; i < list_size(lista); i++)
    {
        t_pcb *pcb = (t_pcb *)list_get(lista, i);
        printf("    PID: %d\n", pcb->PID);
    }
}