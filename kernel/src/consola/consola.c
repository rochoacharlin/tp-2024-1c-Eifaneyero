#include "consola.h"

pthread_t hilo_planificador_largo_plazo;
pthread_t hilo_planificador_corto_plazo;

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
            // COMPLETAR
            inicializar_listas_planificacion();
            t_pcb *pcb = crear_pcb();
            ingresar_pcb_a_NEW(pcb);
        }
        else if (string_starts_with(leido, "FINALIZAR_SCRIPT"))
        {
            PID = atoi(string_substring_from(leido, strlen("FINALIZAR_SCRIPT") + 1));
            // TODO
        }
        else if (strcmp(leido, "DETENER_PLANIFICACION") == 0)
        {
            // REVISAR
            pthread_detach(hilo_planificador_largo_plazo);
            pthread_detach(hilo_planificador_corto_plazo);
            destruir_semaforos_planificacion();
            destruir_listas_planificacion();
        }
        else if (strcmp(leido, "INICIAR_PLANIFICACION") == 0)
        {
            // REVISAR
            inicializar_semaforos_planificacion();

            if (pthread_create(&hilo_planificador_largo_plazo, NULL, (void *)planificar_a_largo_plazo, NULL))
                log_error(logger, "Error creando el hilo del planificador de largo plazo");
            if (pthread_create(&hilo_planificador_corto_plazo, NULL, (void *)planificar_a_corto_plazo_segun_algoritmo, NULL))
                log_error(logger, "Error creando el hilo del planificador de corto plazo");

            pthread_join(hilo_planificador_largo_plazo, NULL);
            pthread_join(hilo_planificador_corto_plazo, NULL);
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

void listar_procesos_por_estado(char *estado, t_list *lista)
{
    printf("%s:", estado);
    for (int i = 0; i < list_size(lista); i++)
    {
        t_pcb *pcb = (t_pcb *)list_get(lista, i);
        printf("    PID: %d", pcb->PID);
    }
}